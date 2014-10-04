/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#if     qHasFeature_libcurl
#include    <curl/curl.h>
#endif

#include    "../../../Characters/Format.h"
#include    "../../../Characters/String_Constant.h"
#include    "../../../Debug/Trace.h"
#include    "../../../Execution/Exceptions.h"

#include    "Client_libcurl.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Execution;
using   namespace   Stroika::Foundation::IO;
using   namespace   Stroika::Foundation::IO::Network;
using   namespace   Stroika::Foundation::IO::Network::Transfer;




// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1


#if     qHasFeature_libcurl
namespace   {
    struct  ModuleInit_ {
        ModuleInit_ ()
        {
            ::curl_global_init (CURL_GLOBAL_ALL);
        }
    };
    ModuleInit_ sIniter_;
}
#endif





#if     qHasFeature_libcurl
class   Connection_LibCurl::Rep_ : public _IRep {
public:
    Rep_ ();
    Rep_ (const Rep_&) = delete;
    virtual ~Rep_ ();

public:
    nonvirtual  Rep_& operator= (const Rep_&) = delete;

public:
    virtual DurationSecondsType GetTimeout () const override;
    virtual void                SetTimeout (DurationSecondsType timeout) override;
    virtual URL                 GetURL () const override;
    virtual void                SetURL (const URL& url) override;
    virtual void                Close ()    override;
    virtual Response            SendAndRequest (const Request& request) override;

private:
    nonvirtual  void    MakeHandleIfNeeded_ ();

private:
    static      size_t  s_ResponseWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP);
    nonvirtual  size_t  ResponseWriteHandler_ (const Byte* ptr, size_t nBytes);

private:
    static      size_t  s_ResponseHeaderWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP);
    nonvirtual  size_t  ResponseHeaderWriteHandler_ (const Byte* ptr, size_t nBytes);

private:
    void*                   fCurlHandle_;
    string                  fCURLCacheUTF8_URL_;    // cuz of quirky memory management policies of libcurl
    vector<Byte>            fResponseData_;
    Mapping<String, String> fResponseHeaders_;
    curl_slist*             fSavedHeaders_;
};
#endif





#if     qHasFeature_libcurl
namespace   {
    wstring mkExceptMsg_ (LibCurlException::CURLcode ccode)
    {
        return String::FromUTF8 (curl_easy_strerror (static_cast<CURLcode> (ccode))).As<wstring> ();
    }
}

/*
 ********************************************************************************
 ************************ Transfer::LibCurlException ****************************
 ********************************************************************************
 */
LibCurlException::LibCurlException (CURLcode ccode)
    : StringException (mkExceptMsg_ (ccode))
    , fCurlCode_ (ccode)
{
}

void    LibCurlException::DoThrowIfError (CURLcode status)
{
    if (status != CURLE_OK) {
        DbgTrace (L"In LibCurlException::DoThrowIfError: throwing status %d (%s)", status, LibCurlException (status).As<String> ().c_str ());
        Execution::DoThrow (LibCurlException (status));
    }
}
#endif








#if     qHasFeature_libcurl
/*
 ********************************************************************************
 ****************** Transfer::Connection_LibCurl::Rep_ **************************
 ********************************************************************************
 */
Connection_LibCurl::Rep_::Rep_ ()
    : fCurlHandle_ (nullptr)
    , fCURLCacheUTF8_URL_ ()
    , fResponseData_ ()
    , fSavedHeaders_ (nullptr)
{
}

Connection_LibCurl::Rep_::~Rep_ ()
{
    if (fCurlHandle_ != nullptr) {
        curl_easy_cleanup (fCurlHandle_);
    }
    if (fSavedHeaders_ != nullptr) {
        curl_slist_free_all (fSavedHeaders_);
        fSavedHeaders_ = nullptr;
    }
}

DurationSecondsType Connection_LibCurl::Rep_::GetTimeout () const
{
    AssertNotImplemented ();
    return 0;
}

void    Connection_LibCurl::Rep_::SetTimeout (DurationSecondsType timeout)
{
    MakeHandleIfNeeded_ ();
    LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_TIMEOUT_MS, static_cast<int> (timeout * 1000)));
    LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_CONNECTTIMEOUT_MS, static_cast<int> (timeout * 1000)));
}

URL     Connection_LibCurl::Rep_::GetURL () const
{
    // needs work... - not sure this is safe - may need to cache orig... instead of reparsing...
    return URL (String::FromUTF8 (fCURLCacheUTF8_URL_).As<wstring> ());
}

void    Connection_LibCurl::Rep_::SetURL (const URL& url)
{
    MakeHandleIfNeeded_ ();
    fCURLCacheUTF8_URL_ = String (url.GetFullURL ()).AsUTF8 ();
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("Connection_LibCurl::Rep_::SetURL ('%s')", fCURLCacheUTF8_URL_.c_str ());
#endif
    LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_URL, fCURLCacheUTF8_URL_.c_str ()));
}

void    Connection_LibCurl::Rep_::Close ()
{
    if (fCurlHandle_ != nullptr) {
        ::curl_easy_cleanup (fCurlHandle_);
        fCurlHandle_ = nullptr;
    }
}

size_t  Connection_LibCurl::Rep_::s_ResponseWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP)
{
    return reinterpret_cast<Rep_*> (userP)->ResponseWriteHandler_ (reinterpret_cast<const Byte*> (ptr), size * nmemb);
}

size_t  Connection_LibCurl::Rep_::ResponseWriteHandler_ (const Byte* ptr, size_t nBytes)
{
    fResponseData_.insert (fResponseData_.end (), ptr, ptr + nBytes);
    return nBytes;
}

size_t  Connection_LibCurl::Rep_::s_ResponseHeaderWriteHandler_ (void* ptr, size_t size, size_t nmemb, void* userP)
{
    return reinterpret_cast<Rep_*> (userP)->ResponseHeaderWriteHandler_ (reinterpret_cast<const Byte*> (ptr), size * nmemb);
}

size_t  Connection_LibCurl::Rep_::ResponseHeaderWriteHandler_ (const Byte* ptr, size_t nBytes)
{
    String  from    =   String::FromUTF8 (reinterpret_cast<const char*> (ptr), reinterpret_cast<const char*> (ptr) + nBytes);
    String  to;
    size_t  i       =   from.find (':');
    if (i != string::npos) {
        to = from.SubString (i + 1);
        from = from.SubString (0, i);
    }
    from = from.Trim ();
    to = to.Trim ();
    fResponseHeaders_.Add (from, to);
    return nBytes;
}

Response    Connection_LibCurl::Rep_::SendAndRequest (const Request& request)
{
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (SDKSTR ("Connection_LibCurl::Rep_::SendAndRequest"));
    DbgTrace (L"(method='%s')", request.fMethod.c_str ());
#endif
    MakeHandleIfNeeded_ ();
    fResponseData_.clear ();
    fResponseHeaders_.clear ();

    //grab useragent from request headers...
    //curl_easy_setopt (fCurlHandle_, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    // grab initial headers and do POST/etc based on args in request...
    curl_slist* tmpH    =   nullptr;
    for (auto i : request.fOverrideHeaders) {
        tmpH = curl_slist_append (tmpH, (i.fKey + String_Constant (L": ") + i.fValue).AsUTF8 ().c_str ());
    }
    AssertNotNull (fCurlHandle_);
    LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_HTTPHEADER, tmpH));
    if (fSavedHeaders_ != nullptr) {
        curl_slist_free_all (fSavedHeaders_);
        fSavedHeaders_ = nullptr;
    }
    fSavedHeaders_ = tmpH;

    LibCurlException::DoThrowIfError (:: curl_easy_perform (fCurlHandle_));

    Response    response;
    response.fData = fResponseData_;

    long    resultCode  =   0;
    LibCurlException::DoThrowIfError (::curl_easy_getinfo (fCurlHandle_, CURLINFO_RESPONSE_CODE, &resultCode));
    response.fStatus = resultCode;
    response.fHeaders = fResponseHeaders_;
    response.fData = fResponseData_;
#if     USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"returning status = %d, dataLen = %d", response.fStatus, response.fData.size ());
#endif
    return response;
}

void    Connection_LibCurl::Rep_::MakeHandleIfNeeded_ ()
{
    if (fCurlHandle_ == nullptr) {
        ThrowIfNull (fCurlHandle_ = ::curl_easy_init ());

        /*
         * Now setup COMMON options we ALWAYS set.
         */
        LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_URL, fCURLCacheUTF8_URL_.c_str ()));

        LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_WRITEFUNCTION, s_ResponseWriteHandler_));
        LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_WRITEDATA, this));
        LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_HEADERFUNCTION, s_ResponseHeaderWriteHandler_));
        LibCurlException::DoThrowIfError (::curl_easy_setopt (fCurlHandle_, CURLOPT_WRITEHEADER, this));
    }
}
#endif







#if     qHasFeature_libcurl
/*
 ********************************************************************************
 ********************** Transfer::Connection_LibCurl ****************************
 ********************************************************************************
 */
Connection_LibCurl::Connection_LibCurl ()
    : Connection (shared_ptr<_IRep> (DEBUG_NEW Rep_ ()))
{
}
#endif
