/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_    1

#include    "../../StroikaPreComp.h"

#include    <type_traits>
#include    <typeindex>

#include    "../../Characters/String.h"
#include    "../../Configuration/Enumeration.h"
#include    "../../Containers/Bijection.h"
#include    "../../Containers/Collection.h"
#include    "../../Containers/Mapping.h"
#include    "../../Containers/Sequence.h"
#include    "../../Containers/Set.h"
#include    "../../Containers/SortedCollection.h"
#include    "../../Containers/SortedMapping.h"
#include    "../../Containers/SortedSet.h"
#include    "../../Execution/Synchronized.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/Optional.h"
#include    "../../Traversal/DiscreteRange.h"
#include    "../../Traversal/Range.h"

#include    "ObjectReader.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo    gross hacks - replciatigng basic objectreaders to get this limping along...

 */



namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {


                using   Characters::String;
                using   Containers::Mapping;
                using   Containers::Sequence;
                using   Containers::Set;
                using   Memory::Byte;


                /**
                 * BuiltinReader<> is not implemented for all types - just for the specialized ones listed below:
                 *      String
                 *      int
                 *      Time::DateTime
                 */
                template    <typename   T>
                class   BuiltinReader : public ObjectReader::IContextReader {
                public:
                    BuiltinReader (T* intoVal);

                private:
                    T* value_;

                public:
                    virtual void    HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader::Context& r) override;
                };
                template    <>
                class   BuiltinReader<String>;
                template    <>
                class   BuiltinReader<int>;
                template    <>
                class   BuiltinReader<unsigned int>;
                template    <>
                class   BuiltinReader<bool>;
                template    <>
                class   BuiltinReader<float>;
                template    <>
                class   BuiltinReader<double>;
                template    <>
                class   BuiltinReader<Time::DateTime>;


                /**
                 *  OptionalTypesReader supports reads of optional types. This will work - for any types for
                 *  which BuiltinReader<T> is implemented.
                 *
                 *  Note - this ALWAYS produces a result. Its only called when the element in quesiton has
                 *  already occurred. The reaosn for Optional<> part is because the caller had an optional
                 *  element which might never have triggered the invocation of this class.
                 */
                template    <typename   T, typename ACTUAL_READER = BuiltinReader<T>>
                class   OptionalTypesReader : public ObjectReader::IContextReader {
                public:
                    OptionalTypesReader (Memory::Optional<T>* intoVal);

                private:
                    Memory::Optional<T>*    value_;
                    T                       proxyValue_;
                    ACTUAL_READER           actualReader_;  // this is why its crucial this partial specialization is only used on optional of types a real reader is available for

                public:
                    virtual void    HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader::Context& r) override;
                };


                /**
                 *  Push one of these Nodes onto the stack to handle 'reading' a node which is not to be read.
                 *  This is necessary to balance out the Start Tag / End Tag combinations.
                 */
                class   IgnoreNodeReader : public ObjectReader::IContextReader {
                public:
                    IgnoreNodeReader ();
                private:
                    int fDepth_;
                public:
                    virtual void    HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader::Context& r) override;
                };


                /**
                 *  Helper class for reading complex (structured) objects.
                 */
                template    <typename   T>
                class   ComplexObjectReader : public ObjectReader::IContextReader {
                protected:
                    ComplexObjectReader (T* vp);

                public:
                    T*  fValuePtr;

                public:
                    virtual void    HandleTextInside (ObjectReader::Context& r, const String& text) override;
                    virtual void    HandleEndTag (ObjectReader::Context& r) override;
                protected:
                    nonvirtual  void    _PushNewObjPtr (ObjectReader::Context& r, const shared_ptr<IContextReader>& newlyAllocatedObject2Push);
                };


                /**
                 *  The ListOfObjectReader<> template can be used to create a vector of type "T" -
                 *  to capture repeating elements in a sequence.
                 *
                 *  EXAMPLE TRAITS:
                 *      struct  ReaderTraits {
                 *              using   ElementType     =   String;
                 *              using   ReaderType      =   BuiltinReader<String>;
                 *              static  const wchar_t           ElementName[] =  L"Name";
                 *      };
                 *
                 *  @todo REPLACE THIS TRAITS API WITH A FACTORY BUILDING NEW READER_OF_T
                 */
                template    <typename TRAITS>
                struct  ListOfObjectReader: public ComplexObjectReader<vector<typename TRAITS::ElementType>> {
                public:
                    ListOfObjectReader (vector<typename TRAITS::ElementType>* v, UnknownSubElementDisposition unknownEltDisposition = UnknownSubElementDisposition::eEndObject);

                    virtual void HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override;
                    virtual void HandleEndTag (ObjectReader::Context& r) override;

                private:
                    typename TRAITS::ElementType                fCurTReading_;
                    shared_ptr<typename TRAITS::ReaderType>     fCurReader_;
                    UnknownSubElementDisposition                fUnknownSubElementDisposition_;
                };



                /**
                // Look back to DataExchange::ObjectVariantmapper, but for now - KISS
                 */
                class   ObjectReaderRegistry {
                public:
                    using ReaderFromVoidStarFactory = function<shared_ptr<ObjectReader::IContextReader> (void*)>;

                public:
                    void    Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory)
                    {
                        fFactories_.Add (forType, readerFactory);
                    }
                    template    <typename T>
                    void    Add (const function<shared_ptr<ObjectReader::IContextReader> (T*)>& readerFactory)
                    {
                        Add (typeid (T), [readerFactory] (void* data) { return readerFactory (reinterpret_cast<T*> (data)); });
                    }

                public:
                    shared_ptr<ObjectReader::IContextReader>    MakeContextReader (type_index ti, void* data)
                    {
                        ReaderFromVoidStarFactory  factory = *fFactories_.Lookup (ti); // must be found or caller/assert error
                        return factory (data);
                    }
                    template    <typename T>
                    shared_ptr<ObjectReader::IContextReader>    MakeContextReader (T* data)
                    {
                        return MakeContextReader (typeid (T), data);
                    }

                private:
                    Mapping<type_index, ReaderFromVoidStarFactory> fFactories_;
                };



                template    <typename   T>
                class   ComplexObjectReader2 : public ComplexObjectReader<T> {
                public:
                    ComplexObjectReader2 (ObjectReaderRegistry* objReg, Mapping<String, pair<type_index, size_t>> maps, T* vp)
                        : ComplexObjectReader<T>(vp)
                        , fObjRegistry  (*objReg)
                        , fFieldNameToTypeMap (maps)
                    {
                    }
                    virtual void    HandleChildStart (StructuredStreamEvents::ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override
                    {
                        Optional<pair<type_index, size_t>>   ti = fFieldNameToTypeMap.Lookup (name);
                        if (ti) {
                            Byte*   operatingOnObj = reinterpret_cast<Byte*> (this->fValuePtr);
                            Byte*   operatingOnObjField = operatingOnObj + ti->second;
                            this->_PushNewObjPtr (r, fObjRegistry.MakeContextReader (ti->first, operatingOnObjField));
                        }
                        else if (fThrowOnUnrecongizedelts) {
                            ThrowUnRecognizedStartElt (name);
                        }
                        else {
                            this->_PushNewObjPtr (r, make_shared<IgnoreNodeReader> ());
                        }
                    }

                    ObjectReaderRegistry&                                               fObjRegistry;
                    Mapping<StructuredStreamEvents::Name, pair<type_index, size_t>>     fFieldNameToTypeMap;            // @todo fix to be mapping on Name but need op< etc defined
                    bool                                                                fThrowOnUnrecongizedelts;       // else ignroe
                };
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory mkComplexObjectReader2Factory (ObjectReaderRegistry* objReg, const Mapping<String, pair<type_index, size_t>>& fieldname2Typeamps)
                {
                    return [objReg, fieldname2Typeamps] (void* data) -> shared_ptr<ObjectReader::IContextReader> { return make_shared<ComplexObjectReader2<T>> (objReg, fieldname2Typeamps, reinterpret_cast<T*> (data)); };
                }



                template    <typename ELEMENT_TYPE>
                struct ListOfObjectReader2: public ComplexObjectReader<vector<ELEMENT_TYPE>> {
                    bool                    readingAT_;
                    ELEMENT_TYPE            curTReading_;
                    ObjectReaderRegistry&   fObjRegistry;
                    String                  fName;

                    ListOfObjectReader2 (ObjectReaderRegistry* objReg, const String& name, vector<ELEMENT_TYPE>* v)
                        : ComplexObjectReader<vector<ELEMENT_TYPE>> (v)
                                , readingAT_ (false)
                                , fObjRegistry  (*objReg)
                                , fName  (name)
                    {
                    }
                    virtual void HandleChildStart (ObjectReader::Context& r, const StructuredStreamEvents::Name& name) override
                    {
                        if (name == fName) {
                            if (readingAT_) {
                                Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
                                this->fValuePtr->push_back (curTReading_);
                                readingAT_ = false;
                            }
                            readingAT_ = true;
                            curTReading_ = ELEMENT_TYPE (); // clear because dont' want to keep values from previous elements
                            this->_PushNewObjPtr (r, fObjRegistry.MakeContextReader<ELEMENT_TYPE> (&curTReading_));
                        }
                        else {
                            ThrowUnRecognizedStartElt (name);
                        }
                    }
                    virtual void HandleEndTag (ObjectReader::Context& r) override
                    {
                        if (readingAT_) {
                            Containers::ReserveSpeedTweekAdd1 (*this->fValuePtr);
                            this->fValuePtr->push_back (curTReading_);
                            readingAT_ = false;
                        }
                        ComplexObjectReader<vector<ELEMENT_TYPE>>::HandleEndTag (r);
                    }
                };
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ObjectReaderRegistry.inl"

#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_h_*/
