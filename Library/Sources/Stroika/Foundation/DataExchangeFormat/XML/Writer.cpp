/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    "../../Streams/iostream/BinaryOutputStreamFromOStreamAdapter.h"
#include    "../../Streams/TextOutputStreamBinaryAdapter.h"

#include    "WriterUtils.h"

#include    "Writer.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::DataExchangeFormat;
using   namespace   Stroika::Foundation::DataExchangeFormat::XML;
using   namespace   Stroika::Foundation::Streams;





namespace   {
    void    Indent_ (const TextOutputStream& out, int indentLevel)
    {
        for (int i = 0; i < indentLevel; ++i) {
            out.Write (L"    ");
        }
    }
}
namespace   {
    void    PrettyPrint_ (const Memory::VariantValue& v, const TextOutputStream& out, int indentLevel);
    void    PrettyPrint_ (bool v, const TextOutputStream& out)
    {
        if (v) {
            out.Write (L"true");
        }
        else {
            out.Write (L"false");
        }
    }
    void    PrettyPrint_ (int v, const TextOutputStream& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%d", v);
        out.Write (buf);
    }
    void    PrettyPrint_ (Memory::VariantValue::FloatType v, const TextOutputStream& out)
    {
        wchar_t buf[1024];
        ::swprintf (buf, NEltsOf (buf), L"%f", v);
        Assert (::wcslen (buf) >= 1);
        // trim trailing 0
        for (size_t i = ::wcslen (buf) - 1; buf[i] == '0'; --i) {
            if (i >= 0 and buf[i - 1] != '.') {
                buf[i] = '\0';
            }
        }
        out.Write (buf);
    }
    void    PrettyPrint_ (const String& v, const TextOutputStream& out)
    {
        // @todo need variant of QuoteForXML that ONLY quotes special cahracters, and not fancy (eg japaense, etc)
        // characters
        //
        // then can clean this up
        out.Write (String::FromAscii (QuoteForXML (v)));
    }
    void    PrettyPrint_ (const vector<Memory::VariantValue>& v, const TextOutputStream& out, int indentLevel)
    {
        for (auto i = v.begin (); i != v.end (); ++i) {
            PrettyPrint_ (*i, out, indentLevel + 1);
            out.Write (L"\n");
        }
        Indent_ (out, indentLevel);
    }
    void    PrettyPrint_ (const map<wstring, Memory::VariantValue>& v, const TextOutputStream& out, int indentLevel)
    {
        //@@@@TODO - must validate first legit xml elt args
        out.Write (L"\n");
        for (auto i = v.begin (); i != v.end ();) {
            Indent_ (out, indentLevel);
            out.Write (L"<");
            out.Write (i->first.c_str ());
            out.Write (L">");
            PrettyPrint_ (i->second, out, indentLevel + 1);
            out.Write (L"</");
            out.Write (i->first.c_str ());
            out.Write (L">");
            ++i;
            out.Write (L"\n");
        }
    }
    void    PrettyPrint_ (const Memory::VariantValue& v, const TextOutputStream& out, int indentLevel)
    {
        switch (v.GetType ()) {
            case    Memory::VariantValue::Type::eNull:
                break;
            case    Memory::VariantValue::Type::eBoolean:
                PrettyPrint_ (v.As<bool> (), out);
                break;
            case    Memory::VariantValue::Type::eDate:
                PrettyPrint_ (v.As<String> (), out);
                break;
            case    Memory::VariantValue::Type::eDateTime:
                PrettyPrint_ (v.As<String> (), out);
                break;
            case    Memory::VariantValue::Type::eInteger:
                PrettyPrint_ (v.As<int> (), out);
                break;
            case    Memory::VariantValue::Type::eFloat:
                PrettyPrint_ (v.As<Memory::VariantValue::FloatType> (), out);
                break;
            case    Memory::VariantValue::Type::eString:
                PrettyPrint_ (v.As<String> (), out);
                break;
            case    Memory::VariantValue::Type::eMap:
                PrettyPrint_ (v.As<map<wstring, Memory::VariantValue>> (), out, indentLevel);
                break;
            case    Memory::VariantValue::Type::eArray:
                PrettyPrint_ (v.As<vector<Memory::VariantValue>> (), out, indentLevel);
                break;
            default:
                RequireNotReached ();       // only certain types allowed
        }
    }
}



/*
 ********************************************************************************
 ******************** DataExchangeFormat::XML::Writer ***************************
 ********************************************************************************
 */
class   DataExchangeFormat::XML::Writer::Rep_ : public DataExchangeFormat::Writer::_IRep {
public:
    DECLARE_USE_BLOCK_ALLOCATION (Rep_);
public:
    Rep_ (const SerializationConfiguration& config)
        : fSerializationConfiguration_ (config)
        , fDocumentElementName_ (config.GetDocumentElementName ().Value ()) {
    }

    virtual void    Write (const Memory::VariantValue& v, const Streams::BinaryOutputStream& out) override {
        if (fDocumentElementName_.empty ()) {
            Require (v.GetType () == Memory::VariantValue::Type::eMap);
            PrettyPrint_ (v, TextOutputStreamBinaryAdapter (out, TextOutputStreamBinaryAdapter::Format::eUTF8WithoutBOM), 0);
        }
        else {
            Containers::Mapping<String, Memory::VariantValue> v2;
            v2.Add (fDocumentElementName_, v);
            PrettyPrint_ (v2, TextOutputStreamBinaryAdapter (out, TextOutputStreamBinaryAdapter::Format::eUTF8WithoutBOM), 0);
        }
    }
    virtual void    Write (const Memory::VariantValue& v, const Streams::TextOutputStream& out) override {
        if (fDocumentElementName_.empty ()) {
            Require (v.GetType () == Memory::VariantValue::Type::eMap);
            PrettyPrint_ (v, out, 0);
        }
        else {
            Containers::Mapping<String, Memory::VariantValue> v2;
            v2.Add (fDocumentElementName_, v);
            PrettyPrint_ (v2, out, 0);
        }
    }
    nonvirtual  SerializationConfiguration GetConfiguration () const {
        return fSerializationConfiguration_;
    }
    nonvirtual  void    SetConfiguration (const SerializationConfiguration& config) {
        fSerializationConfiguration_ = config;
        fDocumentElementName_ = config.GetDocumentElementName ().Value ();
    }

private:
    SerializationConfiguration  fSerializationConfiguration_;
    String                      fDocumentElementName_;
};


DataExchangeFormat::XML::Writer::Writer (const SerializationConfiguration& config)
    : inherited (shared_ptr<_IRep> (new Rep_ (config)))
{
}

DataExchangeFormat::XML::Writer::Rep_&   DataExchangeFormat::XML::Writer::GetRep_ ()
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<Rep_&> (inherited::_GetRep ());
}

const DataExchangeFormat::XML::Writer::Rep_&   DataExchangeFormat::XML::Writer::GetRep_ () const
{
    EnsureMember (&inherited::_GetRep (), Rep_);
    return reinterpret_cast<const Rep_&> (inherited::_GetRep ());
}

SerializationConfiguration DataExchangeFormat::XML::Writer::GetConfiguration () const
{
    return GetRep_ ().GetConfiguration ();
}

void    DataExchangeFormat::XML::Writer::SetConfiguration (const SerializationConfiguration& config)
{
    GetRep_ ().SetConfiguration (config);
}
