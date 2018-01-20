/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <sstream>

#include "../Characters/FloatConversion.h"
#include "../Characters/Format.h"
#include "../Characters/String2Int.h"
#include "../Characters/String_Constant.h"
#include "../Characters/ToString.h"
#include "../Cryptography/Encoding/Algorithm/Base64.h"
#include "../DataExchange/BadFormatException.h"
#include "../Math/Common.h"

#include "VariantValue.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

using Characters::String_Constant;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    // Mimic private class values - but cannot reference directly cuz private and this isn't part of the class
    using IntegerType_         = long long int;
    using UnsignedIntegerType_ = unsigned long long int;
    using FloatType_           = long double;

    // simple mappings to make use of TIRep_<> private template
    template <typename T>
    struct TN_ {
    };
    template <>
    struct TN_<bool> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eBoolean};
    };
    template <>
    struct TN_<Memory::BLOB> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eBLOB};
    };
    template <>
    struct TN_<IntegerType_> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eInteger};
    };
    template <>
    struct TN_<UnsignedIntegerType_> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eUnsignedInteger};
    };
    template <>
    struct TN_<FloatType_> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eFloat};
    };
    template <>
    struct TN_<Date> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eDate};
    };
    template <>
    struct TN_<DateTime> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eDateTime};
    };
    template <>
    struct TN_<String> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eString};
    };
    template <>
    struct TN_<Sequence<VariantValue>> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eArray};
    };
    template <>
    struct TN_<Mapping<String, VariantValue>> {
        static constexpr VariantValue::Type kTYPEENUM{VariantValue::Type::eMap};
    };
}

template <typename T>
struct VariantValue::TIRep_ : VariantValue::IRep_ {
    inline TIRep_ (const T& v)
        : fVal (v)
    {
    }
    inline TIRep_ (T&& v)
        : fVal (move (v))
    {
    }
    virtual Type GetType () const override
    {
        return TN_<T>::kTYPEENUM;
    }
    T fVal;
    DECLARE_USE_BLOCK_ALLOCATION (TIRep_);
};

/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            constexpr EnumNames<DataExchange::VariantValue::Type> DefaultNames<DataExchange::VariantValue::Type>::k;
        }
    }
}

/*
 ********************************************************************************
 ******************************** VariantValue **********************************
 ********************************************************************************
 */
VariantValue::VariantValue (bool val)
    : fVal_{MakeSharedPtr_<TIRep_<bool>> (val)}
{
}

VariantValue::VariantValue (const Memory::BLOB& val)
    : fVal_{MakeSharedPtr_<TIRep_<Memory::BLOB>> (val)}
{
}

VariantValue::VariantValue (signed char val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (short int val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (int val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (long int val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (long long int val)
    : fVal_{MakeSharedPtr_<TIRep_<IntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned char val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned short int val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned int val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned long int val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (unsigned long long int val)
    : fVal_{MakeSharedPtr_<TIRep_<UnsignedIntegerType_>> (val)}
{
}

VariantValue::VariantValue (float val)
    : fVal_{MakeSharedPtr_<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (double val)
    : fVal_{MakeSharedPtr_<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (long double val)
    : fVal_{MakeSharedPtr_<TIRep_<FloatType_>> (val)}
{
}

VariantValue::VariantValue (const Date& val)
    : fVal_{MakeSharedPtr_<TIRep_<Date>> (val)}
{
}

VariantValue::VariantValue (const DateTime& val)
    : fVal_{MakeSharedPtr_<TIRep_<DateTime>> (val)}
{
}

VariantValue::VariantValue (const wchar_t* val)
    : fVal_{MakeSharedPtr_<TIRep_<String>> (val)}
{
}

VariantValue::VariantValue (const wstring& val)
    : fVal_{MakeSharedPtr_<TIRep_<String>> (val)}
{
}

VariantValue::VariantValue (const String& val)
    : fVal_{MakeSharedPtr_<TIRep_<String>> (val)}
{
}

VariantValue::VariantValue (const map<wstring, VariantValue>& val)
    : fVal_{MakeSharedPtr_<TIRep_<Mapping<String, VariantValue>>> (Mapping<String, VariantValue>{val})}
{
}

VariantValue::VariantValue (const Mapping<String, VariantValue>& val)
    : fVal_{MakeSharedPtr_<TIRep_<Mapping<String, VariantValue>>> (val)}
{
}

VariantValue::VariantValue (Mapping<String, VariantValue>&& val)
    : fVal_{MakeSharedPtr_<TIRep_<Mapping<String, VariantValue>>> (move (val))}
{
}

VariantValue::VariantValue (const Sequence<VariantValue>& val)
    : fVal_{MakeSharedPtr_<TIRep_<Sequence<VariantValue>>> (val)}
{
}

VariantValue::VariantValue (const Traversal::Iterable<VariantValue>& val)
    : fVal_{MakeSharedPtr_<TIRep_<Sequence<VariantValue>>> (val)}
{
}

VariantValue::VariantValue (Sequence<VariantValue>&& val)
    : fVal_{MakeSharedPtr_<TIRep_<Sequence<VariantValue>>> (move (val))}
{
}

bool VariantValue::empty () const
{
    if (fVal_ == nullptr) {
        return true;
    }
    switch (fVal_->GetType ()) {
        case Type::eBoolean:
        case Type::eInteger:
        case Type::eUnsignedInteger: {
            return false; // cannot be empty
        }
        case Type::eBLOB: {
            auto v = dynamic_cast<const TIRep_<Memory::BLOB>*> (fVal_.get ());
            return v->fVal.empty ();
        }
        case Type::eFloat: {
            auto v = dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            return std::isnan (v->fVal) != 0;
        }
        case Type::eDate: {
            auto v = dynamic_cast<const TIRep_<Date>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.empty ();
        }
        case Type::eDateTime: {
            auto v = dynamic_cast<const TIRep_<DateTime>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.empty ();
        }
        case Type::eString: {
            auto v = dynamic_cast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.empty ();
        }
        case Type::eMap: {
            auto v = dynamic_cast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.empty ();
        }
        case Type::eArray: {
            auto v = dynamic_cast<const TIRep_<Sequence<VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.empty ();
        }
        default: {
            return As<String> ().empty ();
        }
    }
}

String VariantValue::ToString () const
{
    return As<String> ();
}

template <>
bool VariantValue::As () const
{
    if (fVal_ == nullptr) {
        return false;
    }
    switch (fVal_->GetType ()) {
        case Type::eBoolean: {
            auto v = dynamic_cast<const TIRep_<bool>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            //return tmp != L"false";           // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
            return As<String> () == String_Constant (L"true"); // no need to worry about case etc - cuz XML-Schema  xs:boolean is case-sensative
        }
        case Type::eInteger: {
            return As<IntegerType_> () != 0;
        }
        case Type::eUnsignedInteger: {
            return As<UnsignedIntegerType_> () != 0;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-bool: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to bool")));
        }
    }
}

template <>
Memory::BLOB VariantValue::As () const
{
    if (fVal_ == nullptr) {
        return Memory::BLOB{};
    }
    switch (fVal_->GetType ()) {
        case Type::eBLOB: {
            auto v = dynamic_cast<const TIRep_<Memory::BLOB>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            return Cryptography::Encoding::Algorithm::DecodeBase64 (As<String> ());
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-blob: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to Memory::BLOB")));
        }
    }
}

VariantValue::IntegerType_ VariantValue::AsInteger_ () const
{
    if (fVal_ == nullptr) {
        return 0;
    }
    switch (fVal_->GetType ()) {
        case Type::eFloat: {
            auto v = dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            return Math::Round<VariantValue::IntegerType_> (v->fVal);
        }
        case Type::eInteger: {
            auto v = dynamic_cast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eUnsignedInteger: {
            auto v = dynamic_cast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            auto v = dynamic_cast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return Characters::String2Int<IntegerType_> (v->fVal);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-int: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to integer")));
        }
    }
}

VariantValue::UnsignedIntegerType_ VariantValue::AsUnsignedInteger_ () const
{
    if (fVal_ == nullptr) {
        return 0;
    }
    switch (fVal_->GetType ()) {
        case Type::eFloat: {
            auto v = dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            return Math::Round<VariantValue::UnsignedIntegerType_> (v->fVal);
        }
        case Type::eInteger: {
            auto v = dynamic_cast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eUnsignedInteger: {
            auto v = dynamic_cast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            auto v = dynamic_cast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            // not sure this is right for high order bit set for unsigned long long?
            // --LGP 2013-08-25
            return Characters::String2Int<UnsignedIntegerType_> (v->fVal);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-uint: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to unsigned integer")));
        }
    }
}

VariantValue::FloatType_ VariantValue::AsFloatType_ () const
{
    if (fVal_ == nullptr) {
        return Math::nan<FloatType_> ();
    }
    switch (fVal_->GetType ()) {
        case Type::eInteger: {
            auto v = dynamic_cast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return static_cast<FloatType_> (v->fVal);
        }
        case Type::eUnsignedInteger: {
            auto v = dynamic_cast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            return static_cast<FloatType_> (v->fVal);
        }
        case Type::eFloat: {
            auto v = dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            auto v = dynamic_cast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            // NB. this may return NAN if string not a well-formed number (including empty string case)
            return Characters::String2Float<FloatType_> (v->fVal);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-float: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to float")));
        }
    }
}

template <>
Date VariantValue::As () const
{
    if (fVal_ == nullptr) {
        return Date ();
    }
    switch (fVal_->GetType ()) {
        case Type::eDate: {
            auto v = dynamic_cast<const TIRep_<Date>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eDateTime: {
            auto v = dynamic_cast<const TIRep_<DateTime>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.GetDate ();
        }
        case Type::eString: {
            auto v = dynamic_cast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return Date::Parse (v->fVal, Date::ParseFormat::eISO8601);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-date: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to date")));
        }
    }
}

template <>
DateTime VariantValue::As () const
{
    if (fVal_ == nullptr) {
        return DateTime ();
    }
    switch (fVal_->GetType ()) {
        case Type::eDate: {
            auto v = dynamic_cast<const TIRep_<Date>*> (fVal_.get ());
            AssertNotNull (v);
            return DateTime (v->fVal);
        }
        case Type::eDateTime: {
            auto v = dynamic_cast<const TIRep_<DateTime>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eString: {
            auto v = dynamic_cast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return DateTime::Parse (v->fVal, DateTime::ParseFormat::eISO8601);
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-datetime: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to date-time")));
        }
    }
}

String VariantValue::AsString_ () const
{
    if (fVal_ == nullptr) {
        return String ();
    }
    switch (fVal_->GetType ()) {
        case Type::eNull: {
            AssertNotReached ();
            return String ();
        }
        case Type::eBoolean: {
            auto v = dynamic_cast<const TIRep_<bool>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal ? String_Constant (L"true") : String_Constant (L"false");
        }
        case Type::eBLOB: {
            auto v = dynamic_cast<const TIRep_<Memory::BLOB>*> (fVal_.get ());
            AssertNotNull (v);
            return String::FromAscii (Cryptography::Encoding::Algorithm::EncodeBase64 (v->fVal));
        }
        case Type::eInteger: {
            auto v = dynamic_cast<const TIRep_<IntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            Assert (typeid (v->fVal) == typeid (long long));
            return Characters::Format (L"%lld", v->fVal);
        }
        case Type::eUnsignedInteger: {
            auto v = dynamic_cast<const TIRep_<UnsignedIntegerType_>*> (fVal_.get ());
            AssertNotNull (v);
            Assert (typeid (v->fVal) == typeid (unsigned long long));
            return Characters::Format (L"%llu", v->fVal);
        }
        case Type::eFloat: {
            auto v = dynamic_cast<const TIRep_<FloatType_>*> (fVal_.get ());
            AssertNotNull (v);
            using namespace Characters;
            /*
                 *  numeric_limits<FloatType_>::digits gives the number of digits of the mantissa in radix units, but since we
                 *  write the numbers as decimal, this doesn't come to an integral number of decimal digits.
                 *
                 *  So we either write too many digits of precision, or too few.
                 *
                 *  VERY VERY unsure what is best. From 2013-11-17 until 2015-08-27 I wrote digits10 + 2.
                 *
                 *  As of 2015-08-27 - I'm switching to digits10 + 1 - so effectively just rounding up instead of down.
                 *  This means we will always write all the precision we have and then some ;-).
                 *
                 *  For a bit, I'll leave behind the older comment, which I think is not 100% right, but we can lose it soon.
                    // given a number of digits of precision, its fractional (and rounded down). digits10 + 1 gives you the number
                    // of actual digits after the decimal point. But there is one before the decimal point to give the precision we
                    // use in iostream.
                    //
                    // Acutally - I'm really not sure of any of this. But this seems to work for now...
                    //      -- LGP 2013-11-17
                 */
            //
            //
            //????              static  const   Float2StringOptions kFmtOptions_ { Float2StringOptions::Precision (numeric_limits<FloatType_>::digits10 + 1) };
            static const Float2StringOptions kFmtOptions_{Float2StringOptions::Precision (numeric_limits<FloatType_>::digits10 + 2)};
            return Float2String (v->fVal, kFmtOptions_);
        }
        case Type::eDate: {
            auto v = dynamic_cast<const TIRep_<Date>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.Format (Date::PrintFormat::eISO8601);
        }
        case Type::eDateTime: {
            auto v = dynamic_cast<const TIRep_<DateTime>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal.Format (DateTime::PrintFormat::eISO8601);
        }
        case Type::eString: {
            auto v = dynamic_cast<const TIRep_<String>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        case Type::eArray: {
            auto v = dynamic_cast<const TIRep_<Sequence<VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            wstringstream tmp;
            tmp << L"[";
            for (auto i = v->fVal.begin (); i != v->fVal.end (); ++i) {
                if (i != v->fVal.begin ()) {
                    tmp << L", ";
                }
                tmp << i->As<wstring> ();
            }
            tmp << L"]";
            return tmp.str ();
        }
        case Type::eMap: {
            auto v = dynamic_cast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            wstringstream tmp;
            tmp << L"{";
            for (auto i = v->fVal.begin (); i != v->fVal.end (); ++i) {
                if (i != v->fVal.begin ()) {
                    tmp << L", ";
                }
                tmp << i->fKey.As<wstring> () << L" -> " << i->fValue.As<wstring> ();
            }
            tmp << L"}";
            return tmp.str ();
        }
        default: {
            AssertNotReached (); // That was all types enumerated, and all types convertable to string
            return String ();
        }
    }
}

template <>
map<wstring, VariantValue> VariantValue::As () const
{
    if (fVal_ == nullptr) {
        return map<wstring, VariantValue> ();
    }
    switch (fVal_->GetType ()) {
        case Type::eMap: {
            auto v = dynamic_cast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            map<wstring, VariantValue> tmp;
            for (auto i : v->fVal) {
                tmp.insert (map<wstring, VariantValue>::value_type (i.fKey.As<wstring> (), i.fValue));
            }
            return tmp;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-map<>: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to map")));
        }
    }
}

template <>
Mapping<String, VariantValue> VariantValue::As () const
{
    if (fVal_ == nullptr) {
        return Mapping<String, VariantValue> ();
    }
    switch (fVal_->GetType ()) {
        case Type::eMap: {
            auto v = dynamic_cast<const TIRep_<Mapping<String, VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-Mapping<>: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to map")));
        }
    }
}

template <>
vector<VariantValue> VariantValue::As () const
{
    return As<Sequence<VariantValue>> ().As<vector<VariantValue>> ();
}

template <>
Sequence<VariantValue> VariantValue::As () const
{
    if (fVal_ == nullptr) {
        return Sequence<VariantValue> ();
    }
    switch (fVal_->GetType ()) {
        case Type::eArray: {
            auto v = dynamic_cast<const TIRep_<Sequence<VariantValue>>*> (fVal_.get ());
            AssertNotNull (v);
            return v->fVal;
        }
        default: {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"failed coerce-to-Sequence<>: type=%s, value=%s", Characters::ToString (fVal_->GetType ()).c_str (), Characters::ToString (*this).c_str ());
#endif
            Execution::Throw (DataExchange::BadFormatException (String_Constant (L"Cannot coerce VariantValue to array")));
        }
    }
}

int VariantValue::Compare (const VariantValue& rhs) const
{
    VariantValue::Type lt = GetType ();
    VariantValue::Type rt = rhs.GetType ();
    switch (lt) {
        case VariantValue::Type::eNull:
            return rt == VariantValue::Type::eNull ? 0 : 1;
        case VariantValue::Type::eBoolean:
            return Common::ComparerWithWellOrder<bool>::Compare (As<bool> (), rhs.As<bool> ());
        case VariantValue::Type::eInteger:
            return Common::ComparerWithWellOrder<IntegerType_>::Compare (As<IntegerType_> (), rhs.As<IntegerType_> ());
        case VariantValue::Type::eUnsignedInteger:
            return Common::ComparerWithWellOrder<UnsignedIntegerType_>::Compare (As<UnsignedIntegerType_> (), rhs.As<UnsignedIntegerType_> ());
        case VariantValue::Type::eFloat: {
            // explicit test so we can do NearlyEquals()
            FloatType_ l = As<FloatType_> ();
            FloatType_ r = rhs.As<FloatType_> ();
            if (Math::NearlyEquals (l, r)) {
                return 0;
            }
            else if (l < r) {
                return -1;
            }
            else {
                return 1;
            }
        }
        case VariantValue::Type::eDate:
            return Common::ComparerWithWellOrder<Date>::Compare (As<Date> (), rhs.As<Date> ());
        case VariantValue::Type::eDateTime:
            return Common::ComparerWithWellOrder<DateTime>::Compare (As<DateTime> (), rhs.As<DateTime> ());
        case VariantValue::Type::eString:
            return Common::ComparerWithWellOrder<String>::Compare (As<String> (), rhs.As<String> ());
        case VariantValue::Type::eArray:
            return Common::ComparerWithWellOrder<Sequence<VariantValue>>::Compare (As<Sequence<VariantValue>> (), rhs.As<Sequence<VariantValue>> ());
        case VariantValue::Type::eMap: {
// Cannot do cuz Keys() NYI
// @todo - fix!!!
#if 0
                return Common::ComparerWithWellOrder<Sequence<VariantValue>>::Compare (As<Mapping<String, VariantValue>> ().Keys (), rhs.As<Mapping<String, VariantValue>>.Keys () ());
#endif
            // same iff all elts same
            Mapping<String, VariantValue> lhsM{As<Mapping<String, VariantValue>> ()};
            Mapping<String, VariantValue> rhsM{rhs.As<Mapping<String, VariantValue>> ()};
            auto                          li = lhsM.begin ();
            auto                          ri = rhsM.begin ();
            for (; li != lhsM.end (); ++li, ++ri) {
                if (ri == rhsM.end ()) {
                    return -1;
                }
                if (*li != *ri) {
                    return false;
                }
            }
            Ensure (li == lhsM.end ());
            if (ri == rhsM.end ()) {
                return 0;
            }
            else {
                return 1;
            }
        }
        default:
            AssertNotReached ();
            return false;
    }
}

bool VariantValue::Equals (const VariantValue& rhs, bool exactTypeMatchOnly) const
{
    VariantValue::Type lt = GetType ();
    VariantValue::Type rt = rhs.GetType ();
    if (lt != rt) {
        if (exactTypeMatchOnly) {
            return false;
        }
        else if (
            (lt == VariantValue::Type::eInteger or lt == VariantValue::Type::eUnsignedInteger) and
            (rt == VariantValue::Type::eInteger or rt == VariantValue::Type::eUnsignedInteger)) {
            // Set compare as type and fall through
            lt = VariantValue::Type::eUnsignedInteger;
            rt = VariantValue::Type::eUnsignedInteger;
        }
        else if (
            (lt == VariantValue::Type::eFloat and (rt == VariantValue::Type::eString or rt == VariantValue::Type::eInteger or rt == VariantValue::Type::eUnsignedInteger)) or
            (rt == VariantValue::Type::eFloat and (lt == VariantValue::Type::eString or lt == VariantValue::Type::eInteger or lt == VariantValue::Type::eUnsignedInteger))) {
            // Set compare as type and fall through
            lt = VariantValue::Type::eFloat;
            rt = VariantValue::Type::eFloat;
        }
        else if (
            ((lt != VariantValue::Type::eArray and lt != VariantValue::Type::eMap) and rt == VariantValue::Type::eString) or
            ((rt != VariantValue::Type::eArray and rt != VariantValue::Type::eMap) and lt == VariantValue::Type::eString)) {
            // special case - comparing a string with any type except map or array, trat as strings
            // Set compare as type and fall through
            lt = VariantValue::Type::eString;
            rt = VariantValue::Type::eString;
        }
        else {
            return false;
        }
        // can fall through for some fallthrough cases above...
    }
    switch (lt) {
        case VariantValue::Type::eNull:
            return true;
        case VariantValue::Type::eBoolean:
            return As<bool> () == rhs.As<bool> ();
        case VariantValue::Type::eInteger:
            return As<IntegerType_> () == rhs.As<IntegerType_> ();
        case VariantValue::Type::eUnsignedInteger:
            return As<UnsignedIntegerType_> () == rhs.As<UnsignedIntegerType_> ();
        case VariantValue::Type::eFloat:
            return Math::NearlyEquals (As<VariantValue::FloatType_> (), rhs.As<VariantValue::FloatType_> ());
        case VariantValue::Type::eDate:
            return As<Date> () == rhs.As<Date> ();
        case VariantValue::Type::eDateTime:
            return As<DateTime> () == rhs.As<DateTime> ();
        case VariantValue::Type::eString:
            return As<String> () == rhs.As<String> ();
        case VariantValue::Type::eArray: {
            // same iff all elts same
            Sequence<VariantValue> lhsV = As<Sequence<VariantValue>> ();
            Sequence<VariantValue> rhsV = rhs.As<Sequence<VariantValue>> ();
            if (lhsV.size () != rhsV.size ()) {
                return false;
            }
            for (size_t i = 0; i < lhsV.size (); ++i) {
                if (lhsV[i] != rhsV[i]) {
                    return false;
                }
            }
            return true;
        }
        case VariantValue::Type::eMap: {
            // same iff all elts same
            Mapping<String, VariantValue> lhsM = As<Mapping<String, VariantValue>> ();
            Mapping<String, VariantValue> rhsM = rhs.As<Mapping<String, VariantValue>> ();
            if (lhsM.size () != rhsM.size ()) {
                return false;
            }
            auto li = lhsM.begin ();
            auto ri = rhsM.begin ();
            for (; li != lhsM.end (); ++li, ++ri) {
                if (*li != *ri) {
                    return false;
                }
            }
            Ensure (li == lhsM.end ());
            Ensure (ri == rhsM.end ());
            return true;
        }
        default:
            AssertNotReached ();
            return false;
    }
}
