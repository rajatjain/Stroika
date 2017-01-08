/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_inl_
#define _Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Characters/String2Int.h"
#include    "../../Characters/String_Constant.h"
#include    "../../Characters/ToString.h"
#include    "../BadFormatException.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   DataExchange {
            namespace   StructuredStreamEvents {



                /*
                 ********************************************************************************
                 ********************** ObjectReaderRegistry::IElementConsumer ******************
                 ********************************************************************************
                 */
                template    <typename TARGET_TYPE, typename READER, typename... ARGS>
                ObjectReaderRegistry::ReaderFromVoidStarFactory ObjectReaderRegistry::IElementConsumer::AsFactory (ARGS&& ... args)
                {
                    return ObjectReaderRegistry::ConvertReaderToFactory<TARGET_TYPE, READER> (forward<ARGS> (args)...);
                }


                /*
                 ********************************************************************************
                 ********************** ObjectReaderRegistry::Context ***************************
                 ********************************************************************************
                 */
                inline   ObjectReaderRegistry::Context::Context (const ObjectReaderRegistry& objectReaderRegistry)
                    : fObjectReaderRegistry_ (objectReaderRegistry)
                {
                }
                inline   ObjectReaderRegistry::Context::Context (Context&& from)
                    : fObjectReaderRegistry_ (from.fObjectReaderRegistry_)
                    , fStack_ (move (from.fStack_))
                {
                }
                inline  void    ObjectReaderRegistry::Context::Push (const shared_ptr<IElementConsumer>& elt)
                {
                    RequireNotNull (elt);
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                    if (fTraceThisReader) {
                        DbgTrace (L"%sContext::Push [%s]", TraceLeader_ ().c_str (), Characters::ToString (typeid (*elt.get ())).c_str ());
                    }
#endif
                    Containers::ReserveSpeedTweekAdd1 (fStack_);
                    fStack_.push_back (elt);
                    elt->Activated (*this);
                }
                inline  void    ObjectReaderRegistry::Context::Pop ()
                {
                    fStack_.back()->Deactivating ();
                    fStack_.pop_back ();
#if     qStroika_Foundation_DataExchange_StructuredStreamEvents_SupportTracing
                    if (fTraceThisReader) {
                        if (fStack_.empty ()) {
                            DbgTrace (L"%sContext::Popped [empty stack]", TraceLeader_ ().c_str ());
                        }
                        else {
                            DbgTrace (L"%sContext::Popped [back to: %s]", TraceLeader_ ().c_str (), Characters::ToString (typeid (*GetTop ().get ())).c_str ());
                        }
                    }
#endif
                }
                inline  shared_ptr<ObjectReaderRegistry::IElementConsumer>  ObjectReaderRegistry::Context::GetTop () const
                {
                    Require (not fStack_.empty ());
                    return fStack_.back ();
                }
                inline  const   ObjectReaderRegistry&   ObjectReaderRegistry::Context::GetObjectReaderRegistry () const
                {
                    return fObjectReaderRegistry_;
                }
                inline  bool    ObjectReaderRegistry::Context::empty () const
                {
                    return fStack_.empty ();
                }


                /*
                  ********************************************************************************
                  ************* ObjectReaderRegistry::IConsumerDelegateToContext *****************
                  ********************************************************************************
                  */
                inline  ObjectReaderRegistry::IConsumerDelegateToContext::IConsumerDelegateToContext (Context&& r)
                    : fContext (move (r))
                {
                }
                inline  ObjectReaderRegistry::IConsumerDelegateToContext::IConsumerDelegateToContext (const ObjectReaderRegistry& objectReaderRegistry)
                    : fContext (objectReaderRegistry)
                {
                }
                inline  ObjectReaderRegistry::IConsumerDelegateToContext::IConsumerDelegateToContext (const ObjectReaderRegistry& objectReaderRegistry, const shared_ptr<IElementConsumer>& initialTop)
                    : fContext (objectReaderRegistry, initialTop)
                {
                }


                /*
                 ********************************************************************************
                 ************************************ ClassReader *******************************
                 ********************************************************************************
                 */
                template    <typename   T>
                ObjectReaderRegistry::ClassReader<T>::ClassReader (const Mapping<Name, StructFieldMetaInfo>& maps, T* vp)
                    : fValuePtr_ (vp)
                {
                    RequireNotNull (vp);
                    for (Common::KeyValuePair<Name, StructFieldMetaInfo> i : maps) {
                        if (i.fKey.fType == Name::eValue) {
                            fValueFieldMetaInfo_ = i.fValue;
                        }
                        else {
                            fFieldNameToTypeMap_.Add (i.fKey, i.fValue);
                        }
                    }
                }
                template    <typename   T>
                void    ObjectReaderRegistry::ClassReader<T>::Activated (Context& r)
                {
                    Require (fActiveContext_ == nullptr);
                    fActiveContext_ = &r;
                }
                template    <typename   T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::ClassReader<T>::HandleChildStart (const Name& name)
                {
                    RequireNotNull (fActiveContext_);
                    Optional<StructFieldMetaInfo>   ti = fFieldNameToTypeMap_.Lookup (name);
                    if (ti) {
                        Byte*   operatingOnObj      = reinterpret_cast<Byte*> (this->fValuePtr_);
                        Byte*   operatingOnObjField = operatingOnObj + ti->fOffset;
                        return fActiveContext_->GetObjectReaderRegistry ().MakeContextReader (ti->fTypeInfo, operatingOnObjField);
                    }
                    else if (fThrowOnUnrecongizedelts_) {
                        ThrowUnRecognizedStartElt (name);
                    }
                    else {
                        return make_shared<IgnoreNodeReader> ();
                    }
                }
                template    <typename   T>
                void    ObjectReaderRegistry::ClassReader<T>::HandleTextInside (const String& text)
                {
                    RequireNotNull (fActiveContext_);
                    if (fValueFieldMetaInfo_) {
                        Assert (fValueFieldConsumer_ == nullptr);
                        Byte*   operatingOnObj      = reinterpret_cast<Byte*> (this->fValuePtr_);
                        Byte*   operatingOnObjField = operatingOnObj + fValueFieldMetaInfo_->fOffset;
                        fValueFieldConsumer_ = fActiveContext_->GetObjectReaderRegistry ().MakeContextReader (fValueFieldMetaInfo_->fTypeInfo, operatingOnObjField);
                        fValueFieldConsumer_->Activated (*fActiveContext_);
                        fValueFieldMetaInfo_.clear ();
                    }
                    if (fValueFieldConsumer_) {
                        fValueFieldConsumer_->HandleTextInside (text);
                    }
                }
                template    <typename   T>
                void    ObjectReaderRegistry::ClassReader<T>::Deactivating ()
                {
                    RequireNotNull (fActiveContext_);
                    if (fValueFieldConsumer_) {
                        fValueFieldConsumer_->Deactivating ();
                    }
                    fActiveContext_ = nullptr;
                }
                template    <typename   T>
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::ClassReader<T>::AsFactory ()
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> ();
                }


                /*
                 ********************************************************************************
                 ******************* ObjectReaderRegistry::ListOfObjectReader *******************
                 ********************************************************************************
                 */
                template    <typename CONTAINER_OF_T, typename TRAITS>
                inline  ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T, TRAITS>::ListOfObjectReader (CONTAINER_OF_T* v)
                    : fValuePtr_ (v)
                {
                    RequireNotNull (v);
                }
                template    <typename CONTAINER_OF_T, typename TRAITS>
                inline  ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T, TRAITS>::ListOfObjectReader (CONTAINER_OF_T* v, const Name& memberElementName)
                    : fMemberElementName_  (memberElementName)
                    , fValuePtr_ (v)
                {
                    RequireNotNull (v);
                }
                template    <typename CONTAINER_OF_T, typename TRAITS>
                void    ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T, TRAITS>::Activated (Context& r)
                {
                    Require (fActiveContext_ == nullptr);
                    fActiveContext_ = &r;
                }
                template    <typename CONTAINER_OF_T, typename TRAITS>
                shared_ptr<ObjectReaderRegistry::IElementConsumer> ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T, TRAITS>::HandleChildStart (const StructuredStreamEvents::Name& name)
                {
                    RequireNotNull (fActiveContext_);
                    if (fMemberElementName_.IsMissing () or name == *fMemberElementName_) {
                        if (fReadingAT_) {
                            TRAITS::ContainerAdapterAdder::Add (this->fValuePtr_, fCurTReading_);
                            fReadingAT_ = false;
                        }
                        fReadingAT_ = true;
                        fCurTReading_ = ElementType (); // clear because dont' want to keep values from previous elements
                        return fActiveContext_->GetObjectReaderRegistry ().MakeContextReader<ElementType> (&fCurTReading_);
                    }
                    else if (fThrowOnUnrecongizedelts_) {
                        ThrowUnRecognizedStartElt (name);
                    }
                    else {
                        return make_shared<IgnoreNodeReader> ();
                    }
                }
                template    <typename CONTAINER_OF_T, typename TRAITS>
                void    ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T, TRAITS>::Deactivating ()
                {
                    RequireNotNull (fActiveContext_);
                    if (fReadingAT_) {
                        TRAITS::ContainerAdapterAdder::Add (this->fValuePtr_, fCurTReading_);
                        fReadingAT_ = false;
                    }
                    fActiveContext_ = nullptr;
                }
                template    <typename CONTAINER_OF_T, typename TRAITS>
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T, TRAITS>::AsFactory ()
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> ();
                }
                template    <typename CONTAINER_OF_T, typename TRAITS>
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::ListOfObjectReader<CONTAINER_OF_T, TRAITS>::AsFactory (const Name& memberElementName)
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> (memberElementName);
                }


                /*
                 ********************************************************************************
                 ********************* ObjectReaderRegistry::MixinReader ************************
                 ********************************************************************************
                 */
                template    <typename   T>
                Byte*   ObjectReaderRegistry::MixinReader<T>::MixinEltTraits::kDefaultAddressOfSubElementFetcher = [](T* b) { return reinterpret_cast<Byte*> (b);  };
                template    <typename   T>
                inline  ObjectReaderRegistry::MixinReader<T>::MixinEltTraits::MixinEltTraits (const ObjectReaderRegistry::ReaderFromVoidStarFactory& readerFactory, const function<Byte* (T*)>& addressOfSubEltFetcher)
                    : fReaderFactory (readerFactory)
                    , fAddressOfSubElementFetcher (addressOfSubEltFetcher)
                {
                }
                template    <typename   T>
                inline  ObjectReaderRegistry::MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool (const Name& name)>& readsName, const function<Byte* (T*)>& addressOfSubEltFetcher)
                    : fReaderFactory (readerFactory)
                    , fReadsName (readsName)
                    , fAddressOfSubElementFetcher (addressOfSubEltFetcher)
                {
                }
                template    <typename   T>
                inline  ObjectReaderRegistry::MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool ()>& readsText, const function<Byte* (T*)>& addressOfSubEltFetcher)
                    : fReaderFactory (readerFactory)
                    , fReadsText (readsText)
                    , fAddressOfSubElementFetcher (addressOfSubEltFetcher)
                {
                }
                template    <typename   T>
                inline  ObjectReaderRegistry::MixinReader<T>::MixinEltTraits::MixinEltTraits (const ReaderFromVoidStarFactory& readerFactory, const function<bool (const Name& name)>& readsName, const function<bool ()>& readsText, const function<Byte* (T*)>& addressOfSubEltFetcher)
                    : fReaderFactory (readerFactory)
                    , fReadsName (readsName)
                    , fReadsText (readsText)
                    , fAddressOfSubElementFetcher (addressOfSubEltFetcher)
                {
                }

                template    <typename   T>
                ObjectReaderRegistry::MixinReader<T>::MixinReader (const Traversal::Iterable<MixinEltTraits>& mixins, T* vp)
                    : fValuePtr_ (vp)
                    , fMixins_ (mixins)
                {
                    RequireNotNull (vp);
                    for (MixinEltTraits m : mixins) {
                        fMixinReaders_ += m.fReaderFactory (m.fAddressOfSubElementFetcher (vp));
                    }
                }
                template    <typename   T>
                void    ObjectReaderRegistry::MixinReader<T>::Activated (Context& r)
                {
                    Require (fActiveContext_ == nullptr);
                    fActiveContext_ = &r;
                }
                template    <typename   T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::MixinReader<T>::HandleChildStart (const Name& name)
                {
                    RequireNotNull (fActiveContext_);
                    size_t  idx = 0;
                    for (MixinEltTraits m : fMixins_) {
                        if (m.fReadsName (name)) {
                            shared_ptr<IElementConsumer>    reader = fMixinReaders_[idx];
                            if (not fActivatedReaders_.Contains (reader)) {
                                reader->Activated (*fActiveContext_);
                                fActivatedReaders_.Add (reader);
                            }
                            return reader->HandleChildStart (name);
                        }
                        idx++;
                    }
                    return make_shared<ObjectReaderRegistry::IgnoreNodeReader> ();
                }
                template    <typename   T>
                void    ObjectReaderRegistry::MixinReader<T>::HandleTextInside (const String& text)
                {
                    RequireNotNull (fActiveContext_);
                    size_t  idx = 0;
                    for (MixinEltTraits m : fMixins_) {
                        if (m.fReadsText ()) {
                            shared_ptr<IElementConsumer>    reader = fMixinReaders_[idx];
                            if (not fActivatedReaders_.Contains (reader)) {
                                reader->Activated (*fActiveContext_);
                                fActivatedReaders_.Add (reader);
                            }
                            fMixinReaders_[idx]->HandleTextInside (text);
                        }
                        idx++;
                    }
                }
                template    <typename   T>
                void    ObjectReaderRegistry::MixinReader<T>::Deactivating ()
                {
                    RequireNotNull (fActiveContext_);
                    for (shared_ptr<IElementConsumer> reader : fActivatedReaders_) {
                        reader->Deactivating ();
                    }
                    fActiveContext_ = nullptr;
                }
                template    <typename   T>
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MixinReader<T>::AsFactory ()
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> ();
                }


                /*
                 ********************************************************************************
                 ****************** ObjectReaderRegistry::IgnoreNodeReader **********************
                 ********************************************************************************
                 */
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::IgnoreNodeReader::AsFactory ()
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> ();
                }


                /*
                 ********************************************************************************
                 ****************** ObjectReaderRegistry::ReadDownToReader **********************
                 ********************************************************************************
                 */
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::ReadDownToReader::AsFactory ()
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> ();
                }


                /*
                 ********************************************************************************
                 ************** ObjectReaderRegistry::RepeatedElementReader *********************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                inline  ObjectReaderRegistry::RepeatedElementReader<T, TRAITS>::RepeatedElementReader (ContainerType* pv)
                    : fValuePtr_ (pv)
                {
                }
                template    <typename T, typename TRAITS>
                void    ObjectReaderRegistry::RepeatedElementReader<T, TRAITS>::Activated (ObjectReaderRegistry::Context& r)
                {
                    Assert (fActualReader_ == nullptr);
                    fActualReader_ = TRAITS::MakeActualReader (r, &fProxyValue_);
                    fActualReader_->Activated (r);
                }
                template    <typename T, typename TRAITS>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::RepeatedElementReader<T, TRAITS>::HandleChildStart (const Name& name)
                {
                    AssertNotNull (fActualReader_);
                    return fActualReader_->HandleChildStart (name);
                }
                template    <typename T, typename TRAITS>
                void    ObjectReaderRegistry::RepeatedElementReader<T, TRAITS>::HandleTextInside (const String& text)
                {
                    AssertNotNull (fActualReader_);
                    fActualReader_->HandleTextInside (text);
                }
                template    <typename T, typename TRAITS>
                void    ObjectReaderRegistry::RepeatedElementReader<T, TRAITS>::Deactivating ()
                {
                    AssertNotNull (fActualReader_);
                    fActualReader_->Deactivating ();
                    TRAITS::ContainerAdapterAdder::Add (fValuePtr_, fProxyValue_);
                }
                template    <typename T, typename TRAITS>
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::RepeatedElementReader<T, TRAITS>::AsFactory ()
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> ();
                }


                /*
                 ********************************************************************************
                 ***************************** ObjectReaderRegistry *****************************
                 ********************************************************************************
                 */
                inline void    ObjectReaderRegistry::Add (type_index forType, const ReaderFromVoidStarFactory& readerFactory)
                {
                    fFactories_.Add (forType, readerFactory);
                }
                template    <typename T>
                void    ObjectReaderRegistry::Add (const ReaderFromTStarFactory<T>& readerFactory)
                {
                    Add (typeid (T), [readerFactory] (void* data) { return readerFactory (reinterpret_cast<T*> (data)); });
                }
                inline shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::MakeContextReader (type_index ti, void* destinationObject) const
                {
#if     qDebug
                    if (not fFactories_.ContainsKey (ti)) {
                        Debug::TraceContextBumper   ctx ("ObjectReaderRegistry::MakeContextReader");
                        DbgTrace (L"(forTypeInfo = %s) - UnRegistered Type!", Characters::ToString (ti).c_str ());
                    }
#endif
                    ReaderFromVoidStarFactory  factory = *fFactories_.Lookup (ti); // must be found or caller/assert error
                    return factory (destinationObject);
                }
                template    <typename T>
                inline  shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::MakeContextReader (T* destinationObject) const
                {
                    return MakeContextReader (typeid (T), destinationObject);
                }
                template    <typename T, typename... ARGS>
                inline  void    ObjectReaderRegistry::AddCommonType (ARGS&& ... args)
                {
                    Add<T> (MakeCommonReader<T> (forward<ARGS> (args)...));
                }
                inline  shared_ptr<ObjectReaderRegistry::ReadDownToReader>      ObjectReaderRegistry::mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader)
                {
                    return make_shared<ReadDownToReader> (theUseReader);
                }
                inline  shared_ptr<ObjectReaderRegistry::ReadDownToReader>      ObjectReaderRegistry::mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& tagToHandOff)
                {
                    return make_shared<ReadDownToReader> (theUseReader, tagToHandOff);
                }
                inline  shared_ptr<ObjectReaderRegistry::ReadDownToReader>      ObjectReaderRegistry::mkReadDownToReader (const shared_ptr<IElementConsumer>& theUseReader, const Name& contextTag, const Name& tagToHandOff)
                {
                    return make_shared<ReadDownToReader> (make_shared<ReadDownToReader> (theUseReader, tagToHandOff), contextTag);
                }
                template    <typename CLASS>
                void    ObjectReaderRegistry::AddClass (const Mapping<Name, StructFieldMetaInfo>& fieldInfo)
                {
#if     qDebug
                    for (auto kv : fieldInfo) {
                        if (not fFactories_.ContainsKey (kv.fValue.fTypeInfo)) {
                            Debug::TraceContextBumper   ctx ("ObjectReaderRegistry::AddClass");
                            DbgTrace (L"(CLASS=%s field-TypeInfo-not-found = %s, for field named '%s') - UnRegistered Type!", Characters::ToString (typeid (CLASS)).c_str (), Characters::ToString (kv.fValue.fTypeInfo).c_str (), Characters::ToString (kv.fKey).c_str ());
                            RequireNotReached ();
                        }
                    }
#endif
                    Add<CLASS> (MakeClassReader<CLASS> (fieldInfo));
                }
                template    <typename CLASS>
                auto    ObjectReaderRegistry::MakeClassReader (const Mapping<Name, StructFieldMetaInfo>& fieldInfo) -> ReaderFromVoidStarFactory {
                    return [fieldInfo] (void* data) -> shared_ptr<ObjectReaderRegistry::IElementConsumer> { return make_shared<ObjectReaderRegistry::ClassReader<CLASS>> (fieldInfo, reinterpret_cast<CLASS*> (data)); };
                }
                template    <typename T, typename READER, typename... ARGS>
                auto    ObjectReaderRegistry::ConvertReaderToFactory (ARGS&& ... args) -> ReaderFromVoidStarFactory {
                    ObjectReaderRegistry::ReaderFromTStarFactory<T>   tmpFactory      { [args...] (T * o) -> shared_ptr<ObjectReaderRegistry::IElementConsumer> { return make_shared<READER> (o, forward<ARGS> (args)...); } };
                    return [tmpFactory] (void* data) { return tmpFactory (reinterpret_cast<T*> (data)); };
                }
                template    <typename T>
                auto   ObjectReaderRegistry::cvtFactory_ (const ReaderFromTStarFactory<T>& tf) -> ReaderFromVoidStarFactory {
                    return [tf] (void* data) { return tf (reinterpret_cast<T*> (data)); };
                }
                template    <typename T>
                auto   ObjectReaderRegistry::MakeCommonReader_SimpleReader_ () -> ReaderFromVoidStarFactory {
                    return cvtFactory_<T> ([] (T * o) -> shared_ptr<IElementConsumer> { return make_shared<SimpleReader_<T>> (o); });
                }
                template    <typename ENUM_TYPE>
                auto  ObjectReaderRegistry::MakeCommonReader_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap) -> ReaderFromVoidStarFactory {
                    struct   myReader_ : public IElementConsumer {
                        Containers::Bijection<ENUM_TYPE, String>    fNameMap;
                        myReader_ (const Containers::Bijection<ENUM_TYPE, String>& nameMap, ENUM_TYPE * intoVal)
                            : fNameMap (nameMap)
                            , fValue_ (intoVal)
                        {
                            RequireNotNull (intoVal);
                        }
                        Characters::StringBuilder   fBuf_   {};
                        ENUM_TYPE*                  fValue_ {};
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name & name) override
                        {
                            ThrowUnRecognizedStartElt (name);
                        }
                        virtual void                            HandleTextInside (const String & text) override
                        {
                            fBuf_ += text;
                        }
                        virtual void                            Deactivating () override
                        {
                            if (auto optVal = fNameMap.InverseLookup (fBuf_.str ()))
                            {
                                *fValue_ = *optVal;
                            }
                            else {
                                DbgTrace (L"Enumeration ('%s') value '%s' out of range", Characters::ToString (typeid (ENUM_TYPE)).c_str (), fBuf_.str ().c_str ());
                                Execution::Throw (BadFormatException (Characters::String_Constant (L"Enumeration value out of range")));
                            }
                        }
                    };
                    return cvtFactory_<ENUM_TYPE> ([nameMap] (ENUM_TYPE * o) -> shared_ptr<IElementConsumer> { return make_shared<myReader_> (nameMap, o); });
                };
                template    <typename ENUM_TYPE>
                auto  ObjectReaderRegistry::MakeCommonReader_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap) -> ReaderFromVoidStarFactory {
                    return MakeCommonReader_NamedEnumerations (Containers::Bijection<ENUM_TYPE, String> (nameMap));
                }
                template    <typename ENUM_TYPE>
                auto  ObjectReaderRegistry::MakeCommonReader_EnumAsInt () -> ReaderFromVoidStarFactory {
                    struct   myReader_ : public IElementConsumer {
                        myReader_ (ENUM_TYPE * intoVal)
                            : fValue_ (intoVal)
                        {
                            RequireNotNull (intoVal);
                        }
                        Characters::StringBuilder   fBuf_   {};
                        ENUM_TYPE*                  fValue_ {};
                        virtual shared_ptr<IElementConsumer>    HandleChildStart (const Name & name) override
                        {
                            ThrowUnRecognizedStartElt (name);
                        }
                        virtual void                            HandleTextInside (const String & text) override
                        {
                            fBuf_ += text;
                        }
                        virtual void                            Deactivating () override
                        {
                            using   SerializeAsType     =   typename std::underlying_type<ENUM_TYPE>::type;
                            SerializeAsType tmp     =   Characters::String2Int<SerializeAsType> (fBuf_.str ());
                            if (Configuration::ToInt (ENUM_TYPE::eSTART) <= tmp and tmp < Configuration::ToInt (ENUM_TYPE::eEND))
                            {
                                *fValue_ = Configuration::ToEnum<ENUM_TYPE> (tmp);
                            }
                            else {
                                DbgTrace (L"Enumeration ('%s') value '%s' out of range", Characters::ToString (typeid (ENUM_TYPE)).c_str (), fBuf_.str ().c_str ());
                                Execution::Throw (BadFormatException (Characters::String_Constant (L"Enumeration value out of range")));
                            }
                        }
                    };
                    return cvtFactory_<ENUM_TYPE> ([] (ENUM_TYPE * o) -> shared_ptr<IElementConsumer> { return make_shared<myReader_> (o); });
                }
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const String*)
                {
                    return MakeCommonReader_SimpleReader_<String> ();
                }
                template    <typename T>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const T*, typename std::enable_if<std::is_enum<T>::value >::type*)
                {
                    return MakeCommonReader_NamedEnumerations<T> ();
                }
                template    <typename T>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const T*, typename std::enable_if < std::is_pod<T>::value and !std::is_enum<T>::value >::type*)
                {
                    return MakeCommonReader_SimpleReader_<T> ();
                }
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const Time::DateTime*)
                {
                    return MakeCommonReader_SimpleReader_<Time::DateTime> ();
                }
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::MakeCommonReader_ (const Time::Duration*)
                {
                    return MakeCommonReader_SimpleReader_<Time::Duration> ();
                }
                template    <typename T, typename TRAITS>
                inline   ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const Memory::Optional<T, TRAITS>*)
                {
                    return cvtFactory_<Memory::Optional<T>> ([] (Memory::Optional<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<OptionalTypesReader_<T>> (o); });
                }
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const vector<T>*)
                {
                    return cvtFactory_<vector<T>> ([] (vector<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectReader<vector<T>>> (o); });
                }
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const vector<T>*, const Name& name)
                {
                    return cvtFactory_<vector<T>> ([name] (vector<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectReader<vector<T>>> (o, name); });
                }
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const Sequence<T>*)
                {
                    return cvtFactory_<Sequence<T>> ([] (Sequence<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectReader<Sequence<T>>> (o); });
                }
                template    <typename T>
                ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader_ (const Sequence<T>*, const Name& name)
                {
                    return cvtFactory_<Sequence<T>> ([name] (Sequence<T>* o) -> shared_ptr<IElementConsumer> { return make_shared<ListOfObjectReader<Sequence<T>>> (o, name); });
                }
                template    <typename T, typename... ARGS>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory  ObjectReaderRegistry::MakeCommonReader (ARGS&& ... args)
                {
                    const T*  n = nullptr;    // arg unused, just for overloading
                    DISABLE_COMPILER_MSC_WARNING_START (6011)
                    return MakeCommonReader_ (n, forward<ARGS> (args)...);
                    DISABLE_COMPILER_MSC_WARNING_END (6011)
                }


                /*
                 ********************************************************************************
                 ******************* ObjectReaderRegistry::SimpleReader_ ************************
                 ********************************************************************************
                 */
                template    <typename   T>
                inline  ObjectReaderRegistry::SimpleReader_<T>::SimpleReader_ (T* intoVal)
                    : fValue_ (intoVal)
                {
                    RequireNotNull (intoVal);
                }
                template    <typename   T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::SimpleReader_<T>::HandleChildStart (const StructuredStreamEvents::Name& name)
                {
                    ThrowUnRecognizedStartElt (name);
                }
                template    <typename   T>
                void    ObjectReaderRegistry::SimpleReader_<T>::HandleTextInside (const String& text)
                {
                    fBuf_ += text;
                }
                template    <typename   T>
                void   ObjectReaderRegistry::SimpleReader_<T>::Deactivating ()
                {
#if     qCompilerAndStdLib_StaticAssertionsInTemplateFunctionsWhichShouldNeverBeExpanded_Buggy
                    RequireNotReached ();
#else
                    static_assert (false, "Only specifically specialized variants are supported");
#endif
                }
                template    <typename   T>
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::SimpleReader_<T>::AsFactory ()
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> ();
                }


                /*
                 ********************************************************************************
                 ************* ObjectReaderRegistry::OptionalTypesReader_<T> ********************
                 ********************************************************************************
                 */
                template    <typename   T>
                inline  ObjectReaderRegistry::OptionalTypesReader_<T>::OptionalTypesReader_ (Memory::Optional<T>* intoVal)
                    : fValue_ (intoVal)
                {
                    RequireNotNull (intoVal);
                }
                template    <typename   T>
                void    ObjectReaderRegistry::OptionalTypesReader_<T>::Activated (Context& r)
                {
                    Assert (fActualReader_ == nullptr);
                    fActualReader_ = r.GetObjectReaderRegistry ().MakeContextReader (&fProxyValue_);
                    fActualReader_->Activated (r);
                }
                template    <typename   T>
                shared_ptr<ObjectReaderRegistry::IElementConsumer>    ObjectReaderRegistry::OptionalTypesReader_<T>::HandleChildStart (const StructuredStreamEvents::Name& name)
                {
                    AssertNotNull (fActualReader_);
                    return fActualReader_->HandleChildStart (name);
                }
                template    <typename   T>
                void    ObjectReaderRegistry::OptionalTypesReader_<T>::HandleTextInside (const String& text)
                {
                    AssertNotNull (fActualReader_);
                    fActualReader_->HandleTextInside (text);
                }
                template    <typename   T>
                void    ObjectReaderRegistry::OptionalTypesReader_<T>::Deactivating ()
                {
                    AssertNotNull (fActualReader_);
                    fActualReader_->Deactivating ();
                    *fValue_ = fProxyValue_;
                }
                template    <typename   T>
                template    <typename TARGET_TYPE, typename READER>
                inline  ObjectReaderRegistry::ReaderFromVoidStarFactory   ObjectReaderRegistry::OptionalTypesReader_<T>::AsFactory ()
                {
                    return IElementConsumer::AsFactory<TARGET_TYPE, READER> ();
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_DataExchange_StructuredStreamEvents_ObjectReaderRegistry_inl_*/
