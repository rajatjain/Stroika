/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_AnyVariantValue_inl_
#define _Stroika_Foundation_Memory_AnyVariantValue_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            struct  AnyVariantValue::IRep_ {
                virtual ~IRep_ () {}
                virtual const type_info&    GetType () const    =   0;
            };

            template    <typename T>
            struct  AnyVariantValue::TIRep_ : public IRep_ {
                T   fValue;
                TIRep_ (const T& from):
                    fValue (from)
                {}
                virtual const type_info&    GetType () const override {
                    return typeid (T);
                }
            };


            /*
             ********************************************************************************
             ******************************* AnyVariantValue ********************************
             ********************************************************************************
             */
            inline  AnyVariantValue::AnyVariantValue ()
                : fVal_ ()
            {
            }
            template    <typename   T>
            inline  AnyVariantValue::AnyVariantValue (T val)
                : fVal_ (shared_ptr<IRep_> (new TIRep_<T> (val)))
            {
            }
#if     !qCompilerAndStdLib_Supports_ExplicitlyDeletedSpecialMembers
            inline  AnyVariantValue::AnyVariantValue (const AnyVariantValue& from)
                : fVal_ (from.fVal_)
            {
            }
            inline  const AnyVariantValue& AnyVariantValue::operator= (const AnyVariantValue& rhs)
            {
                fVal_ = rhs.fVal_;
                return *this;
            }
#endif
            inline  const type_info&  AnyVariantValue::GetType () const
            {
                if (fVal_.get () == nullptr) {
                    return typeid (void);
                }
                return fVal_->GetType ();
            }
            inline  bool    AnyVariantValue::empty () const
            {
                return fVal_.get () == nullptr;
            }
            inline  void    AnyVariantValue::clear ()
            {
                fVal_.reset ();
            }
            template    <typename   RETURNTYPE>
            inline  AnyVariantValue::operator RETURNTYPE () const
            {
                Require (typeid (RETURNTYPE) == GetType ());
                Require (typeid (RETURNTYPE) != typeid (void));
                AssertNotNull (fVal_.get ());
                // Could use dynamic_cast but this should be equally safe (cuz of assert above) and more
                // efficient
                const TIRep_<RETURNTYPE>*   t   =   reinterpret_cast<TIRep_<RETURNTYPE>*> (fVal_.get ());
                return t->fValue;
            }
            bool    AnyVariantValue::Equals (const AnyVariantValue& rhs) const
            {
                if (empty ()) {
                    return rhs.empty ();
                }
                AssertNotImplemented ();    // I don't need this for now - add virtual method to TIRep_!!!
                return false;
            }
            inline  bool    AnyVariantValue::operator== (const AnyVariantValue& rhs) const
            {
                return Equals (rhs);
            }
            inline  bool    AnyVariantValue::operator!= (const AnyVariantValue& rhs) const
            {
                return not Equals (rhs);
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Memory_VariantValue_inl_*/
