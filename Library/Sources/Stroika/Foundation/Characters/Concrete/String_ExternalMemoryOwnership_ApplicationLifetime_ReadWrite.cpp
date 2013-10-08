/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdarg>
#include    <climits>
#include    <istream>
#include    <string>
#include    <regex>

#include    "../../Containers/Common.h"
#include    "../../Execution/Exceptions.h"
#include    "../../Math/Common.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/BlockAllocated.h"

#include    "Private/String_ReadWriteRep.h"
#include    "Private/String_BufferedStringRep.h"

#include    "String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite.h"





using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Characters;
using   namespace   Stroika::Foundation::Characters::Concrete;




namespace   {
    class   String_BufferedArray_Rep_ : public Concrete::Private::BufferedStringRep::_Rep {
    private:
        typedef Concrete::Private::BufferedStringRep::_Rep  inherited;
    public:
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end)
            : inherited (start, end) {
        }
        String_BufferedArray_Rep_ (const wchar_t* start, const wchar_t* end, size_t reserve)
            : inherited (start, end, reserve) {
        }
        virtual shared_ptr<_IRep>   Clone () const override {
            return shared_ptr<_IRep> (DEBUG_NEW String_BufferedArray_Rep_ (_fStart, _fEnd));
        }
    public:
        DECLARE_USE_BLOCK_ALLOCATION(String_BufferedArray_Rep_);
    };
}








/*
 * TODO:
 *      o   COULD do better - saving ORIGNIAL BUFFER SIZE - in addition to memory range.
 *          Right now - this class takes a big buffer (possibly) and lets you modify it, and possibly shrunk the string, but if you ever try to insert,
 *          its forgotten its original size (bufsize), and so it mallocs a new buffer (by thorwing unsupported).
 *
 *          Not a biggie issue for now since this class really isn't used (much).
 *              -- LGP 2011-12-03
 *
 */
class   String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::MyRep_ : public Concrete::Private::ReadWriteRep::_Rep {
private:
    typedef Concrete::Private::ReadWriteRep::_Rep   inherited;
public:
    MyRep_ (wchar_t* start, wchar_t* end)
        : inherited (start, end) {
    }
    virtual shared_ptr<_IRep>   Clone () const override {
        /*
         * Subtle point - but since this code involves SHARING buffer space, we cannot have two different string reps both sharing the same pointer. Only
         * one can use it, and the other must make a copy.
         */
        return shared_ptr<_IRep> (DEBUG_NEW String_BufferedArray_Rep_ (_fStart, _fEnd));
    }
public:
    DECLARE_USE_BLOCK_ALLOCATION(MyRep_);
};
















/*
 ********************************************************************************
 ********** String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite ********
 ********************************************************************************
 */
String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite::String_ExternalMemoryOwnership_ApplicationLifetime_ReadWrite (wchar_t* cString)
    : String (_SharedRepByValuePtr::shared_ptr_type (DEBUG_NEW MyRep_ (cString, cString + wcslen (cString))))
{
}
