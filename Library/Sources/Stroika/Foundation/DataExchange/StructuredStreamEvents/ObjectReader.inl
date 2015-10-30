/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_inl_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_inl_ 1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Containers/Common.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


                /*
                 ********************************************************************************
                 ************************************* ObjectReader *****************************
                 ********************************************************************************
                 */
                inline  ObjectReader::ObjectReader ()
#if     qDefaultTracingOn
                    : fTraceThisReader (false)
#endif
                {
                }
                inline  void    ObjectReader::Push (const shared_ptr<ObjectBase>& elt)
                {
#if     qDefaultTracingOn
                    if (fTraceThisReader) {
                        DbgTrace ("%sObjectReader::Push", TraceLeader_ ().c_str ());
                    }
#endif
                    Containers::ReserveSpeedTweekAdd1 (fStack_);
                    fStack_.push_back (elt);
                }
                inline  void    ObjectReader::Pop ()
                {
                    fStack_.pop_back ();
#if     qDefaultTracingOn
                    if (fTraceThisReader) {
                        DbgTrace ("%sObjectReader::Popped", TraceLeader_ ().c_str ());
                    }
#endif
                }
                inline  shared_ptr<ObjectReader::ObjectBase>  ObjectReader::GetTop () const
                {
                    Require (not fStack_.empty ());
                    return fStack_.back ();
                }


                /*
                 ********************************************************************************
                 ********************************** BuiltinReader *******************************
                 ********************************************************************************
                 */
                template    <>
                class   BuiltinReader<String> : public ObjectReader::ObjectBase {
                public:
                    BuiltinReader (String* intoVal);
                private:
                    String* value_;
                public:
                    virtual void    HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader& r) override;
                };
                template    <>
                class   BuiltinReader<int> : public ObjectReader::ObjectBase {
                public:
                    BuiltinReader (int* intoVal);
                private:
                    String  tmpVal_;
                    int*    value_;
                public:
                    virtual void    HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader& r) override;
                };
                template    <>
                class   BuiltinReader<unsigned int> : public ObjectReader::ObjectBase {
                public:
                    BuiltinReader (unsigned int* intoVal);
                private:
                    String  tmpVal_;
                    unsigned int*   value_;
                public:
                    virtual void    HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader& r) override;
                };
                template    <>
                class   BuiltinReader<float> : public ObjectReader::ObjectBase {
                public:
                    BuiltinReader (float* intoVal);
                private:
                    String  tmpVal_;
                    float*  value_;
                public:
                    virtual void    HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader& r) override;
                };
                template    <>
                class   BuiltinReader<double> : public ObjectReader::ObjectBase {
                public:
                    BuiltinReader (double* intoVal);
                private:
                    String  tmpVal_;
                    double* value_;
                public:
                    virtual void    HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader& r) override;
                };
                template    <>
                class   BuiltinReader<bool> : public ObjectReader::ObjectBase {
                public:
                    BuiltinReader (bool* intoVal);
                private:
                    String  tmpVal_;
                    bool*   value_;
                public:
                    virtual void    HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader& r) override;
                };
                template    <>
                class   BuiltinReader<Time::DateTime> : public ObjectReader::ObjectBase {
                public:
                    BuiltinReader (Time::DateTime* intoVal);
                private:
                    String          tmpVal_;
                    Time::DateTime* value_;
                public:
                    virtual void    HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader& r) override;
                };


                /*
                 ********************************************************************************
                 ******************** OptionalTypesReader<T, ACTUAL_READER> *********************
                 ********************************************************************************
                 */
                template    <typename   T, typename ACTUAL_READER>
                OptionalTypesReader<T, ACTUAL_READER>::OptionalTypesReader (Memory::Optional<T>* intoVal)
                    : value_ (intoVal)
                    , proxyValue_ ()
                    , actualReader_ (&proxyValue_)
                {
                }
                template    <typename   T, typename ACTUAL_READER>
                void    OptionalTypesReader<T, ACTUAL_READER>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
                {
                    actualReader_.HandleChildStart (r, name);
                }
                template    <typename   T, typename ACTUAL_READER>
                void    OptionalTypesReader<T, ACTUAL_READER>::HandleTextInside (ObjectReader& r, const String& text)
                {
                    actualReader_.HandleTextInside (r, text);
                }
                template    <typename   T, typename ACTUAL_READER>
                void    OptionalTypesReader<T, ACTUAL_READER>::HandleEndTag (ObjectReader& r)
                {
                    shared_ptr<ObjectBase>   saveCopyOfUs        =   r.GetTop ();    // bump our reference count til the end of the procedure
                    // because the HandleEndTag will typically cause a POP on the reader that destroys us!
                    // However, we cannot do the copy back to value beofre the base POP, because
                    // it also might do some additioanl processing on its value
                    actualReader_.HandleEndTag (r);
                    *value_ = proxyValue_;
                }


                /*
                 ********************************************************************************
                 ******************************* ComplexObjectReader<T> *************************
                 ********************************************************************************
                 */
                template    <typename   T>
                inline  ComplexObjectReader<T>::ComplexObjectReader (T* vp)
                    : fValuePtr (vp)
                {
                    RequireNotNull (vp);
                }
                template    <typename   T>
                void    ComplexObjectReader<T>::HandleTextInside (ObjectReader& r, const String& text)
                {
                    // OK so long as text is whitespace - or comment. Probably should check/assert, but KISS..., and count on validation to
                    // assure input is valid
                    Assert (text.IsWhitespace ());
                }
                template    <typename   T>
                void    ComplexObjectReader<T>::HandleEndTag (ObjectReader& r)
                {
                    r.Pop ();
                }
                template    <typename   T>
                void    ComplexObjectReader<T>::_PushNewObjPtr (ObjectReader& r, const shared_ptr<ObjectBase>& newlyAllocatedObject2Push)
                {
                    RequireNotNull (newlyAllocatedObject2Push);
                    r.Push (newlyAllocatedObject2Push);
                }


                /*
                 ********************************************************************************
                 *************************** ListOfObjectReader<TRAITS> *************************
                 ********************************************************************************
                 */
                template    <typename TRAITS>
                ListOfObjectReader<TRAITS>::ListOfObjectReader (vector<typename TRAITS::ElementType>* v)
                    : ComplexObjectReader<vector<typename TRAITS::ElementType>> (v)
                            , readingAT_ (false)
                {
                }
                template    <typename TRAITS>
                void ListOfObjectReader<TRAITS>::HandleChildStart (ObjectReader& r, const StructuredStreamEvents::Name& name)
                {
                    if (name.fLocalName == TRAITS::ElementName) {
                        if (readingAT_) {
                            Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
                            this->fValuePtr->push_back (curTReading_);
                            readingAT_ = false;
                        }
                        readingAT_ = true;
                        curTReading_ = typename TRAITS::ElementType (); // clear because dont' want to keep values from previous elements
                        this->_PushNewObjPtr (r, make_shared<typename TRAITS::ReaderType> (&curTReading_));
                    }
                    else {
                        ThrowUnRecognizedStartElt (name);
                    }
                }
                template    <typename TRAITS>
                void ListOfObjectReader<TRAITS>::HandleEndTag (ObjectReader& r)
                {
                    if (readingAT_) {
                        Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
                        this->fValuePtr->push_back (curTReading_);
                        readingAT_ = false;
                    }
                    ComplexObjectReader<vector<typename TRAITS::ElementType>>::HandleEndTag (r);
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReader_inl_*/
