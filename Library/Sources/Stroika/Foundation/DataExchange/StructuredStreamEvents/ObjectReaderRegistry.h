/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_ObjectReaderRegistry_h_
#define _Stroika_Foundation_DataExchange_ObjectReaderRegistry_h_    1

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

#include    "../VariantValue.h"



/**
 *
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *
 *      @todo    */



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
                 *  \brief  ObjectReaderRegistry can be used to map C++ types to and from variant-union types, which can be transparently mapped into and out of XML, JSON, etc.
                 *
                 *  ObjectReaderRegistry IS COPYABLE. Make one instance, register your types into it and use this to
                 *  serialized/ToObject
                 *
                 *
                 *  \par Example Usage
                 *      \code
                 *      struct  SharedContactsConfig_ {
                 *          bool                    fEnabled = false;
                 *          DateTime                fLastSynchronizedAt;
                 *          Mapping<String,String>  fThisPHRsIDToSharedContactID;
                 *      };
                 *
                 *      ObjectReaderRegistry mapper;
                 *
                 *      // register each of your mappable (even private) types
                 *      mapper.AddClass<SharedContactsConfig_> ({
                 *          ObjectReaderRegistry_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fEnabled, L"Enabled"),
                 *          ObjectReaderRegistry_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fLastSynchronizedAt, L"Last-Synchronized-At"),
                 *          ObjectReaderRegistry_StructureFieldInfo_Construction_Helper (SharedContactsConfig_, fThisPHRsIDToSharedContactID, L"This-HR-ContactID-To-SharedContactID-Map"),
                 *      });
                 *
                 *      SharedContactsConfig_   tmp;
                 *      tmp.fEnabled = enabled;
                 *      VariantValue v = mapper.Serialize  (tmp);
                 *
                 *      // at this point - we should have VariantValue object with "Enabled" field.
                 *      // This can then be serialized using
                 *
                 *      Streams::MemoryStream<Byte>   tmpStream;
                 *      DataExchange::JSON::PrettyPrint (v, tmpStream);
                 *
                 *      // THEN deserialized, and mapped back to C++ object form
                 *      tmp = mapper.ToObject<SharedContactsConfig_> (DataExchange::JSON::Reader (tmpStream));
                 *      if (tmp.fEnabled) {
                 *          ...
                 *      }
                 *      \endcode
                 */
                class   ObjectReaderRegistry {
                public:
                    /**
                     */
                    using   ToVariantMapperType     =   function<VariantValue(const ObjectReaderRegistry& mapper, const Byte* objOfType)>;

                public:
                    /**
                     */
                    using   FromVariantMapperType   =   function<void(const ObjectReaderRegistry& mapper, const VariantValue& d, Byte* into)>;

                public:
                    /**
                     *  Defaults to installing basic type mappers (@see ResetToDefaultTypeRegistry).
                     */
                    ObjectReaderRegistry ();

                public:
                    /**
                     *  Structure to capture all the details of how to map between a VariantValue and an associated C++ structure.
                     *  This CAN be direclyly constructed, and passed into the ObjectReaderRegistry (via the Add method), but more commonly
                     *  helpers like MakeCommonSerializer () or AddClass will be used.
                     */
                    struct  TypeMappingDetails {
                        type_index              fForType;
                        ToVariantMapperType     fToVariantMapper;
                        FromVariantMapperType   fFromVariantMapper;

                        TypeMappingDetails (const type_index& forTypeInfo, const ToVariantMapperType& toVariantMapper, const FromVariantMapperType& fromVariantMapper);

                        nonvirtual  bool operator== (const TypeMappingDetails& rhs) const;
                        nonvirtual  bool operator< (const TypeMappingDetails& rhs) const;
                    };

                public:
                    struct  TypesRegistry;

                public:
                    /**
                     *  Returns the current set of type mappers.
                     */
                    nonvirtual  TypesRegistry    GetTypeMappingRegistry () const;

                public:
                    /**
                     *  Sets the current set of type mappers.
                     */
                    nonvirtual  void    SetTypeMappingRegistry (const TypesRegistry& s);

                public:
                    /**
                     *  Adds the given type mapper(s). This could have been called 'Merge'.
                     */
                    nonvirtual  void    Add (const TypeMappingDetails& s);
                    nonvirtual  void    Add (const Set<TypeMappingDetails>& s);
                    nonvirtual  void    Add (const TypesRegistry& s);
                    nonvirtual  void    Add (const ObjectReaderRegistry& s);

                public:
                    /**
                     *  Shortcut for Add (MakeCommonSerializer<T> ());
                     *
                     *  So - this is supported for any type for which (@see MakeCommonSerializer) is supported.
                     *
                     *  Note this this is not needed (because it's done by default), but is supported,
                     *  for the builtin types.
                     */
                    template    <typename T>
                    nonvirtual  void    AddCommonType ();

                public:
                    /**
                     *  This clears the registry of type mappers, and resets it to the defaults - a set of builtin types,
                     *  like String, int, etc.
                     *
                     *  Builtin types include:
                     *      o   bool
                     *      o   signed char, short int, int, long int, long long int
                     *      o   unsigned char, unsigned short int, unsigned int, unsigned long int, unsigned long long int
                     *      o   float, double, long double
                     *      o   Date
                     *      o   DateTime
                     *      o   Duration
                     *      o   String
                     *      o   Mapping<String, String>
                     *      o   Mapping<String, VariantValue>
                     *      o   TimeOfDay
                     *      o   VariantValue
                     *      o   DurationRange/DateRange/DateTimeRange
                     *
                     *  @todo - IT SOON WILL CONTAIN Sequence<String>, and Sequence<Variant>, as well as vector<> of those types.
                     *
                     *  Note - to include any of your user-defined types (structs) - you must use Add () or
                     *  AddClass ().
                     *
                     *  @see GetTypeMappingRegistry
                     *  @see SetTypeMappingRegistry
                     *  @see AddClass
                     *  @see Add
                     *  @see MakeCommonSerializer
                     */
                    nonvirtual  void    ResetToDefaultTypeRegistry ();

                public:
                    struct  StructureFieldInfo;

                public:
                    /**
                     *  Adds the given class (defined in explicit template argument) with the given list of field.
                     *  Also, optionally provide a 'readPreflight' function to be applied to the read-in VariantValue object before
                     *  decomposing (into C++ structs), as a helpful backward compatible file format hook.
                     */
                    template    <typename CLASS>
                    nonvirtual  void    AddClass (const Sequence<StructureFieldInfo>& fieldDescriptions);
#if     !qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug
                    template    <typename CLASS>
                    nonvirtual  void    AddClass (const std::initializer_list<StructureFieldInfo>& fieldDescriptions);
#endif
                    template    <typename CLASS>
                    nonvirtual  void    AddClass (const StructureFieldInfo* fieldDescriptionsStart, const StructureFieldInfo* fieldDescriptionsEnd);
                    template    <typename CLASS>
                    nonvirtual  void    AddClass (const Sequence<StructureFieldInfo>& fieldDescriptions, function<void(VariantValue*)> preflightBeforeToObject);
#if     !qCompilerAndStdLib_stdinitializer_ObjectVariantMapperBug
                    template    <typename CLASS>
                    nonvirtual  void    AddClass (const std::initializer_list<StructureFieldInfo>& fieldDescriptions, function<void(VariantValue*)> preflightBeforeToObject);
#endif
                    template    <typename CLASS>
                    nonvirtual  void    AddClass (const StructureFieldInfo* fieldDescriptionsStart, const StructureFieldInfo* fieldDescriptionsEnd, function<void(VariantValue*)> preflightBeforeToObject);

                public:
                    /**
                     *  Returns the function that does the data mapping. This can be used as an optimization to
                     *  avoid multiple lookups of the mapper for a given type (say when reading or writing an array).
                     */
                    template    <typename TYPE>
                    nonvirtual  FromVariantMapperType   ToObjectMapper () const;
                    nonvirtual  FromVariantMapperType   ToObjectMapper (const type_index& forTypeInfo) const;

                public:
                    /**
                     *  Convert a VariantValue object into any C++ object - using the type converters already registered in
                     *  this mapper.
                     *
                     *  The overloads that takes 'fromVariantMapper' are just an optimization, and need not be used, but if used, the value
                     *  passed in MUST the the same as that returned by ToObjectMapper ().
                     */
                    template    <typename TYPE>
                    nonvirtual  TYPE                    ToObject (const VariantValue& v) const;
                    template    <typename TYPE>
                    nonvirtual  void                    ToObject (const VariantValue& v, TYPE* into) const;
                    template    <typename TYPE>
                    nonvirtual  void                    ToObject (const FromVariantMapperType& fromVariantMapper, const VariantValue& v, TYPE* into) const;
                    template    <typename TYPE>
                    nonvirtual  TYPE                    ToObject (const FromVariantMapperType& fromVariantMapper, const VariantValue& v) const;

                public:
                    /**
                     *  Returns the function that does the data mapping. This can be used as an optimization to
                     *  avoid multiple lookups of the mapper for a given type (say when reading or writing an array).
                     */
                    template    <typename TYPE>
                    nonvirtual  ToVariantMapperType     FromObjectMapper () const;
                    nonvirtual  ToVariantMapperType     FromObjectMapper (const type_index& forTypeInfo) const;

                public:
                    /**
                     *  Convert a C++ object to a VariantValue object - using the type converters already registered in
                     *  this mapper.
                     *
                     *  The overload that takes 'toVariantMapper' is just an optimization, and need not be used, but if used, the value
                     *  passed in MUST the the same as that returned by FromObjectMapper ().
                     */
                    template    <typename TYPE>
                    nonvirtual  VariantValue            FromObject (const TYPE& from) const;
                    template    <typename TYPE>
                    nonvirtual  VariantValue            FromObject (const ToVariantMapperType& toVariantMapper, const TYPE& from) const;

                public:
                    /**
                     *  This creates serializers for many common types.
                     *      o   Mapping<Key,Value>
                     *      o   Optional<T>
                     *      o   Range<T,TRAITS>
                     *      o   Sequence<T>
                     *      o   Set<T>
                     *  ###NYI    o   T[N]      -- so far cannot get to work
                     *      o   enum types (with eSTART/eEND @see Stroika_Define_Enum_Bounds for bounds checking)
                     *
                     *  This assumes the template parameters for the above objects are also already defined (mostly 'T' above).
                     *
                     *  This function also works (but is generally unneeded for) any of the types defined in
                     *  @see ResetToDefaultTypeRegistry () (int, short, String, etc).
                     *
                     *  Note - all these de-serializers will throw BadDataFormat exceptions if the data somehow doesnt
                     *  fit what the deserailizer expects.
                     */
                    template    <typename T>
                    static  TypeMappingDetails  MakeCommonSerializer ();

                public:
                    /**
                     */
                    template    <typename ENUM_TYPE>
                    static  TypeMappingDetails  MakeCommonSerializer_NamedEnumerations (const Containers::Bijection<ENUM_TYPE, String>& nameMap);
                    template    <typename ENUM_TYPE>
                    static  TypeMappingDetails  MakeCommonSerializer_NamedEnumerations (const Configuration::EnumNames<ENUM_TYPE>& nameMap);

                public:
                    /**
                     *
                     *  This works on Any Iterable<KeyValuePair<>>, where the Key can be Mapped to / from a String, using
                     *  an already defined typemapper (from KEY_TYPE to/from String) or be of type String.
                     */
                    template    <typename ACTUAL_CONTAINTER_TYPE, typename KEY_TYPE = typename ACTUAL_CONTAINTER_TYPE::KeyType, typename VALUE_TYPE = typename ACTUAL_CONTAINTER_TYPE::ValueType>
                    static  ObjectReaderRegistry::TypeMappingDetails MakeCommonSerializer_ContainerWithStringishKey ();

                private:
                    template    <typename DOMAIN_TYPE, typename RANGE_TYPE, typename TRAITS>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::Bijection<DOMAIN_TYPE, RANGE_TYPE, TRAITS>&);
                    template    <typename T>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::Collection<T>&);
                    template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Mapping<KEY_TYPE, VALUE_TYPE, TRAITS>&);
                    template    <typename T, typename TRAITS>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Memory::Optional<T, TRAITS>&);
                    template    <typename T, typename TRAITS>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Execution::Synchronized<T, TRAITS>&);
                    template    <typename T>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Sequence<T>&);
                    template    <typename T>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const vector<T>&);
                    template    <typename T>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Set<T>&);
                    template    <typename T>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::SortedCollection<T>&);
                    template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>&);
                    template    <typename T>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Containers::SortedSet<T>&);
                    template    <typename T, typename TRAITS>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Traversal::DiscreteRange<T, TRAITS>&);
                    template    <typename T, typename TRAITS>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const Traversal::Range<T, TRAITS>&);
                    template    <typename ENUM_TYPE>
                    static  TypeMappingDetails  MakeCommonSerializer_ (const ENUM_TYPE&,  typename std::enable_if<std::is_enum<ENUM_TYPE>::value >::type* = 0);

                private:
                    template    <typename ACTUAL_CONTAINER_TYPE>
                    static  TypeMappingDetails  MakeCommonSerializer_WithSimpleAdd_ ();

                private:
                    template    <typename ACTUAL_CONTAINER_TYPE>
                    static  TypeMappingDetails  MakeCommonSerializer_WithSimpleAddByAppend_ ();

                private:
                    template    <typename KEY_TYPE, typename VALUE_TYPE, typename ACTUAL_CONTAINER_TYPE>
                    static  TypeMappingDetails  MakeCommonSerializer_WithKeyValuePairAdd_ ();

                private:
                    template    <typename T, size_t SZ>
                    static  TypeMappingDetails MakeCommonSerializer_ (const T (&)[SZ]);

                private:
                    template    <typename RANGE_TYPE>
                    static  ObjectReaderRegistry::TypeMappingDetails MakeCommonSerializer_Range_ ();

                private:
                    nonvirtual  ObjectReaderRegistry::TypeMappingDetails  MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields) const;
                    nonvirtual  ObjectReaderRegistry::TypeMappingDetails  MakeCommonSerializer_ForClassObject_ (const type_index& forTypeInfo, size_t n, const Sequence<StructureFieldInfo>& fields, function<void(VariantValue*)> preflightBeforeToObject) const;

                private:
                    nonvirtual  TypeMappingDetails  Lookup_(const type_index& forTypeInfo) const;

                public:
                    /**
                    */
                    struct  TypesRegistry {
                        Set<TypeMappingDetails> fSerializers;         // need Stroika set with separate traits-based key extractor/compare function
                    };

                private:
                    TypesRegistry   fTypeMappingRegistry_;
                };


                /**
                 *  This is just for use the with the ObjectReaderRegistry::AddClass<> (and related) methods, to describe a user-defined type (CLASS).
                 */
                struct  ObjectReaderRegistry::StructureFieldInfo {
                    enum class NullFieldHandling {
                        eOmit,
                        eInclude
                    };
                    enum class ArrayElementHandling {
                        eExact,
                        eTryExtraArray
                    };

                    size_t                  fOffset;
                    type_index              fTypeInfo;
                    String                  fSerializedFieldName;
                    NullFieldHandling       fNullFields;
                    ArrayElementHandling    fSpecialArrayHandling;

                    StructureFieldInfo (size_t fieldOffset, type_index typeInfo, const String& serializedFieldName, NullFieldHandling nullFields = NullFieldHandling::eInclude, ArrayElementHandling arrayHandling = ArrayElementHandling::eExact);
                };


                /**
                 *  Greatly regret adding a macro, but it just seems SO HELPFUL (makes things much more terse).
                 *  No need to use - but some may find it helpfull...
                 *
                 *  I don't know of any way in C++ without macro - to capture a member name (for use in decltype
                 *  thing and offsetof()).
                 *
                 *  \note
                 *      This macro uses offsetof(). According to the C++11 spec, offsetof() is only supported
                 *      for 'standard-layout' objects (e.g. those without virtual functions, or other fancy
                 *      c++ stuff). As near as I can tell, this always works, but we may need to revisit
                 *      the approach/question (could  we use pointer to member?).
                 *
                 *  @see Stroika_Foundation_DataExchange_ObjectReaderRegistry_FieldInfoKey
                 */
#define     ObjectReaderRegistry_StructureFieldInfo_Construction_Helper(CLASS,MEMBER,NAME)\
    DataExchange::ObjectReaderRegistry::StructureFieldInfo (offsetof (CLASS, MEMBER), typeid (decltype (CLASS::MEMBER)), NAME)


                /**
                 *  Greatly regret adding a macro, but it just seems SO HELPFUL (makes things much more terse).
                 *  No need to use - but some may find it helpfull...
                 *
                 *  I don't know of any way in C++ without macro - to capture a member name (for use in decltype
                 *  thing and offsetof()).
                 *
                 *  \note
                 *      This macro uses offsetof(). According to the C++11 spec, offsetof() is only supported
                 *      for 'standard-layout' objects (e.g. those without virtual functions, or other fancy
                 *      c++ stuff). As near as I can tell, this always works, but we may need to revisit
                 *      the approach/question (could  we use pointer to member?).
                 *
                 *  @see ObjectReaderRegistry_StructureFieldInfo_Construction_Helper
                 */
#define     Stroika_Foundation_DataExchange_ObjectReaderRegistry_FieldInfoKey(CLASS,MEMBER)\
    offsetof (CLASS, MEMBER), typeid (decltype (CLASS::MEMBER))


                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<bool> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<signed char> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<short int> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<int> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<long int> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<long long int> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned char> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned short int> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned int> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned long int> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<unsigned long long int> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<float> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<double> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<long double> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Time::Date> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Time::DateTime> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Characters::String> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<VariantValue> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Time::Duration> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Time::TimeOfDay> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Mapping<Characters::String, Characters::String>> ();
                template    <>
                ObjectReaderRegistry::TypeMappingDetails  ObjectReaderRegistry::MakeCommonSerializer<Mapping<Characters::String, VariantValue>> ();


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

#endif  /*_Stroika_Foundation_DataExchange_ObjectReaderRegistry_h_*/
