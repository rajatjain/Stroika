/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_LRUCache_h_
#define _Stroika_Foundation_Cache_LRUCache_h_  1

#include    "../StroikaPreComp.h"

#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Containers/Mapping.h"
#include    "../Cryptography/Digest/Algorithm/Jenkins.h"
#include    "../Cryptography/Hash.h"
#include    "../Debug/AssertExternallySynchronizedLock.h"
#include    "../Memory/Optional.h"



/**
 *      \file
 *
 * TODO:
 *
 *  @todo   Make sure the Mapping<> is constructed with TRAITS that allow for the proper "EQUALS" method.
 *
 *  @todo   Hash_SFINAE_<> IS HORRIBLE HACK!!!! CLEANUP!!! Tricky... SFINAE -- SEE SerializeForHash1_
 *
 *
 *          >>>> BIG PICTURE TODO - REVIEW THIS TODO LIST CUZ MOST ALREADY DONE NOW, BUT INCOMPLETE SO LEAVE TIL CONVERSION COMPLETE <<<<
 *
 *      @todo   Look at nu_LRUCache<> and consider possability of migrating to it, or something like that
 *              Much slower, but threadsafe, and simpler to use.
 *
 *              Probably migrate older code to class Legacy_LRUCache<>
 *
 *      @todo   TERRIBLE API!
 *                  LRUCache<string> tmp (3);
 *                  *tmp.AddNew ("x") = "x";
 *                  *tmp.AddNew ("y") = "y";
 *                  *tmp.AddNew ("z") = "z";
 *                  *tmp.AddNew ("a") = "a";
 *              So awkward to use!!! - find better - more map like - API
 *
 *      @todo   One major source of UNSAFETY (especially thread unsafety) is
 *              AddNew/Lookup() API returning internal pointers. We really want this to do copy by
 *              value. But look where its used, and see the costs are not too high of doing copy my value.
 *              Or maybe use concext update object (maybe taking a lambda) for the update).
 *
 *      @todo   NOTE - I'm NOT sure public API for this is stable yet - there are many problems (though
 *              I've used this class quite a lot - its not buggy - just not flexibly designed or good
 *              thread safety properties).
 *
 *      @todo   Test and make sure ITERATOR stuff works properly when using Hashing mode (HASTABLE SIZE > 1)
 *
 *      @todo   Currently we have redundant storage - _Buf, and _First, and _Last (really just need _Buf cuz
 *              has first/last, or do our own storage managemnet with secondary array? - we do the mallocs/frees.
 *              To re-free, even though order distorted by shuffles, we can always figure out which was
 *              the original array head by which has the lowest address!
 *
 *              Also somewhat related, _Last usage is C++ unconvnetional - though maybe OK. If not more awkward
 *              in impl, consider using _fEnd? Or if it is (I think last maybe better then document clearly why
 *              its better.
 *
 *      @todo   Need way to produce ITERABLE (copy??? of cache data - so callers can LOOK at what is in teh cache)
 *
 *      @todo   Consider restructuring the API more like STL-MAP
 *              KEY,VALUE, intead of LRUCache<ELEMENT> and the ability to extract an element.
 *              Doing this MIGHT even allow us to make this class fit more neatly within the Stroika container
 *              pantheon. BUt it will be a bit of a PITA for all/some of the existing LRUCache<> uses...
 *
 *              Problem of doing this is that in some cases - we may want to create a cache of objects that
 *              already pre-exist, and have the 'key part' as a subobject of themselves. Not a killer as we
 *              COULD just call the KEY/ELEMENT the same type and just use a simple test traits function
 *              that just pays attention to the logical key parts?
 *
 *      @todo   Cleanup docs to reflect new TRAITS style, and document each public method (still todo) and docs for
 *              main class needs cleanup).
 *
 *              AND docs for main class need a simple example of usage (without a traits override).
 *
 *      @todo   Verify the size of  TRAITS::StatsType   fStats; is zero, or go back to old qKeepLRUCacheStats
 *              macro stuff to avoid wasted space.
 *                  o  Trouble with subobject approach is C++ seems to force all objects to be at least one byte,
 *                      so there WOULD be cost. Could avoid that by having the TRAITS
 *                      OBJECT ITSELF be what owns the counters - basically global vars. Since just
 *                      used for testing, could still be usable that way...
 *
 *      @todo   LRUCache CONSTRUCTS all its elements, even if they arent in the cache, That might be bad. Redo if not
 *              too hard so when items removed from cache (use optional) they arent really there. Cached objects COULD be big.
 *
 *      @todo   Come up with easy way to persist cache. I suppose this counts on fact that you can load/store
 *              cached elements � so persist mechanism must parameterize that).
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {


            namespace   LRUCacheSupport {


                /**
                 *  Helper detail class for analyzing and tuning cache statistics.
                 */
                struct  Stats_Basic {
                    size_t      fCachedCollected_Hits   { };
                    size_t      fCachedCollected_Misses { };
                    void    IncrementHits ();
                    void    IncrementMisses ();
                };


                /**
                 *  Helper for DefaultTraits - when not collecting stats.
                 */
                struct  Stats_Null {
                    void    IncrementHits ();
                    void    IncrementMisses ();
                };


                /**
                 *  Helper for DefaultTraits.
                 */
#if     qDebug
                using   StatsType_DEFAULT   =   Stats_Basic;
#else
                using   StatsType_DEFAULT   =   Stats_Null;
#endif

                /**
                 */
                template    <typename KEY, size_t HASH_TABLE_SIZE = 1>
                struct  DefaultTraits {
                    using   KeyType     =   KEY;

                    // HASHTABLESIZE must be >= 1, but if == 1, then Hash function not used
                    DEFINE_CONSTEXPR_CONSTANT(size_t, kHashTableSize, HASH_TABLE_SIZE);

                    // If KeyType different type than ElementType we need a hash for that too

                    //tmphack - SHOUDL do smarter defaults!!!!
                    template    <typename SFINAE>
                    static  size_t  Hash_SFINAE_ (const KEY& e, typename enable_if < is_arithmetic<SFINAE>::value || is_convertible<SFINAE, string>::value || is_convertible<SFINAE, Characters::String>::value, void >::type* = nullptr)
                    {
                        using   Cryptography::Digest::Digester;
                        using   Cryptography::Digest::Algorithm::Jenkins;
                        using   USE_DIGESTER_     =   Digester<Jenkins>;
                        return Cryptography::Hash<USE_DIGESTER_, KEY, size_t> (e);
                    }
                    template    <typename SFINAE>
                    static  size_t  Hash_SFINAE_ (const KEY& e, typename enable_if < not (is_arithmetic<SFINAE>::value || is_convertible<SFINAE, string>::value || is_convertible<SFINAE, Characters::String>::value), void >::type* = nullptr)
                    {
                        return 0;
                    }
                    static  size_t  Hash (const KEY& e)
                    {
                        return Hash_SFINAE_<KEY> (e);
                    }

                    // defaults to operator==
                    static  bool    Equals (const KEY& lhs, const KEY& rhs)
                    {
                        return lhs == rhs;
                    }

                    using   StatsType   =   LRUCacheSupport::StatsType_DEFAULT;
                };

            }





            /**
             *  LRUCache is NOT threadsafe (checks usage with Debug::AssertExternallySynchronizedLock), so typical uses would use Execution::Synchonized.
             *
             *  EXAMPLE USAGE:
             *      Execution::Synchronized<LRUCache<DetailsID, Details_>>      sDetailsCache_; // caches often helpful in multithreaded situations
             */
            template    <typename KEY, typename VALUE, typename TRAITS = LRUCacheSupport::DefaultTraits<KEY>>
            class   LRUCache : /*private*/public Debug::AssertExternallySynchronizedLock {
            public:
                LRUCache (size_t size = 1);
                LRUCache (const LRUCache& from);

            public:
                nonvirtual  const LRUCache& operator= (const LRUCache& rhs);

            public:
                /**
                 */
                nonvirtual  size_t  GetMaxCacheSize () const;

            public:
                /**
                 */
                nonvirtual  void    SetMaxCacheSize (size_t maxCacheSize);

            public:
                /**
                 */
                nonvirtual  typename TRAITS::StatsType  GetStats () const;

            public:
                /**
                 */
                nonvirtual  void    clear ();
                nonvirtual  void    clear (const KEY& key);
                nonvirtual  void    clear (function<bool(const KEY&)> clearPredicate);

            public:
                /**
                 *  experimental as of 2015-02-01 - maybe use regular optional traits?
                 */
                using   OptionalValue   =   Memory::Optional<VALUE, Memory::Optional_Traits_Blockallocated_Indirect_Storage<VALUE>>;

            public:
                /**
                 *  The value associated with KEY may not be present, so an Optional is returned.
                 *
                 *  @see LookupValue ()
                 */
                nonvirtual  OptionalValue Lookup (const KEY& key) const;

            public:
                /**
                 *  LookupValue () finds the value in the cache, and returns it, or if not present, uses the argument valueFetcher to retrieve it.
                 *
                 *  So LookupValue (v) is equivilent to:
                 *          if (auto o = Lookup (k)) {
                 *              return o;
                 *          }
                 *          else {
                 *              auto v = valueFetcher (k);
                 *              Add (k, v);
                 *              return v;
                 *          }
                 *
                 *  EXAMPLE USAGE:
                 *      struct Details_ {
                 *      };
                 *      using DetailsID = int;
                 *      Details_ ReadDetailsFromFile_ (DetailsID id);
                 *
                 *      Execution::Synchronized<LRUCache<DetailsID, Details_>>      fDetailsCache_; // caches often helpful in multithreaded situations
                 *
                 *      // returns the value from LRUCache, or automatically pages it in from file
                 *      Details_    GetDetails (DetailsID id) {
                 *          return
                 *              fDetailsCache_->LookupValue (
                 *                  id,
                 *                  [] (DetailsID id) -> Details_ { return ReadDetailsFromFile_ (id); }
                 *              );
                 *      }
                 */
                nonvirtual  VALUE   LookupValue (const KEY& key, const function<VALUE(KEY)>& valueFetcher);

            public:
                /**
                 */
                nonvirtual  void    Add (const KEY& key, const VALUE& value);

            public:
                /**
                 */
                nonvirtual  Containers::Mapping<KEY, VALUE>     Elements () const;

            private:
                //tmphack - use optional so we can avoid CTOR rules... and eventually also avoid allocating space? Best to have one outer optinal, butat thjats tricky with current API
                struct  LEGACYLRUCACHEOBJ_ {
                    Memory::Optional<KEY>     fKey;
                    Memory::Optional<VALUE>   fValue;
                };
                struct  LEGACYLRUCACHEOBJ_TRAITS_ {
                    using   ElementType =   LEGACYLRUCACHEOBJ_;
                    using   KeyType     =   KEY;
                    using   StatsType   =   LRUCacheSupport::StatsType_DEFAULT;

                    static  void    Clear (ElementType* element)
                    {
                        (*element) = ElementType ();
                    }
                    static  Memory::Optional<KEY> ExtractKey (const LEGACYLRUCACHEOBJ_& e)
                    {
                        return e.fKey;
                    }
                    DEFINE_CONSTEXPR_CONSTANT(size_t, HASH_TABLE_SIZE, TRAITS::kHashTableSize);
                    static  size_t  HS_ (const KEY& k)
                    {
                        return TRAITS::Hash (k);
                    }
                    static  size_t  Hash (const Memory::Optional<KEY>& e)
                    {
                        static_assert (TRAITS::kHashTableSize >= 1, "HASH_TABLE_SIZE >= 1");
                        if (TRAITS::kHashTableSize == 1) {
                            return 0;   // avoid referencing hash function
                        }
                        else if (e.IsMissing ()) {
                            return 0;
                        }
                        else {
                            return HS_ (*e);
                        }
                    }
                    static  bool    Equal (const Memory::Optional<KEY>& lhs, const Memory::Optional<KEY>& rhs)
                    {
                        if (lhs.IsMissing () != rhs.IsMissing ()) {
                            return false;
                        }
                        if (lhs.IsMissing () and rhs.IsMissing ()) {
                            return true;
                        }
                        return TRAITS::Equals (*lhs, *rhs);
                    }
                };

            private:
                class   LRUCache_ {
                public:
                    using ELEMENT = LEGACYLRUCACHEOBJ_;
                public:
                    using   ElementType     =   typename LEGACYLRUCACHEOBJ_TRAITS_::ElementType;
                    using   KeyType         =   typename LEGACYLRUCACHEOBJ_TRAITS_::KeyType;

                public:
                    LRUCache_ (size_t maxCacheSize);
                    LRUCache_ () = delete;
                    LRUCache_ (const LRUCache_&) = delete;

                public:
                    nonvirtual  LRUCache_& operator= (const LRUCache_&) = delete;

                public:
                    nonvirtual  size_t  GetMaxCacheSize () const;
                    nonvirtual  void    SetMaxCacheSize (size_t maxCacheSize);

                public:
                    struct  CacheIterator;
                public:
                    nonvirtual  CacheIterator   begin ();
                    nonvirtual  CacheIterator   end ();

                public:
                    nonvirtual  void    ClearCache ();

                public:
                    /**
                     *  NOTE - though you can CHANGE the value of ELEMENT, it is illegal to change its KEY part/key
                     *  value if you specified HASH_TABLE_SIZE != 1 in TRAITS object.
                     */
                    nonvirtual  ELEMENT*    AddNew (const KeyType& item);

                public:
                    /*
                     *  NOTE - though you can CHANGE the value of ELEMENT, it is illegal to change its KEY part/key
                     *  value if you specified HASH_TABLE_SIZE != 1 in TRAITS object.
                     */
                    nonvirtual  ELEMENT*    LookupElement (const KeyType& item);

                public:
                    typename LEGACYLRUCACHEOBJ_TRAITS_::StatsType  fStats;

                private:
                    struct  CacheElement_ {
                        CacheElement_*   fNext = nullptr;
                        CacheElement_*   fPrev = nullptr;
                        ElementType      fElement {};
                    };

                public:
                    struct  CacheIterator {
                        explicit CacheIterator (CacheElement_** start, CacheElement_** end)
                            : fCurV (start)
                            , fEndV (end)
                            , fCur (start == end ? nullptr : *fCurV)
                        {
                        }
                        CacheIterator& operator++ ()
                        {
                            RequireNotNull (fCur);
                            Require (fCurV != fEndV);
                            fCur = fCur->fNext;
                            if (fCur == nullptr) {
                                fCurV++;
                                if (fCurV != fEndV) {
                                    fCur  = *fCurV;
                                }
                            }
                            return *this;
                        }
                        ELEMENT& operator* ()
                        {
                            RequireNotNull (fCur);
                            return fCur->fElement;
                        }
                        ELEMENT* operator-> ()
                        {
                            RequireNotNull (fCur);
                            return &fCur->fElement;
                        }
                        bool operator== (CacheIterator rhs)
                        {
                            return fCur == rhs.fCur;
                        }
                        bool operator!= (CacheIterator rhs)
                        {
                            return fCur != rhs.fCur;
                        }

                    private:
                        CacheElement_**  fCurV;
                        CacheElement_**  fEndV;
                        CacheElement_*   fCur;
                    };

                private:
                    vector<CacheElement_>   fCachedElts_BUF_[LEGACYLRUCACHEOBJ_TRAITS_::HASH_TABLE_SIZE];      // we don't directly use these, but use the First_Last pointers instead which are internal to this buf
                    CacheElement_*          fCachedElts_First_[LEGACYLRUCACHEOBJ_TRAITS_::HASH_TABLE_SIZE];
                    CacheElement_*          fCachedElts_Last_[LEGACYLRUCACHEOBJ_TRAITS_::HASH_TABLE_SIZE];

                private:
                    nonvirtual  void    ShuffleToHead_ (size_t chainIdx, CacheElement_* b);
                };

            private:
                mutable LRUCache_  fRealCache_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "LRUCache.inl"

#endif  /*_Stroika_Foundation_Cache_LRUCache_h_*/
