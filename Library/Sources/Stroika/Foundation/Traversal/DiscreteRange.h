/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_h_
#define _Stroika_Foundation_Traversal_DiscreteRange_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterable.h"
#include    "Range.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  TODO:
 *      @todo   Conflict in names begin/end between Iterator and Range<> are INCONVENIENT and UNSESIRBALE.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {



            template    <typename T, typename SIGNED_DIFF_TYPE = size_t, typename UNSIGNED_DIFF_TYPE = size_t>
            struct  DefaultENUM_TESTTRAITS {
                typedef T                   ElementType;
                typedef SIGNED_DIFF_TYPE    SignedDifferenceType;
                typedef UNSIGNED_DIFF_TYPE  UnsignedDifferenceType;

#if     qCompilerAndStdLib_Supports_constexpr_StaticDataMember
                static  constexpr T kMin = T::eSTART;
                static  constexpr T kMax = eEND;
#else
                static  const T kMin;
                static  const T kMax;
#endif
                static bool GetNext (T* n) {
                    //tmphack
                    *n = static_cast<T> (static_cast<int> (*n) + 1);
                    return true;
                }
            };
#if     !qCompilerAndStdLib_Supports_constexpr_StaticDataMember
            template    <typename T, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T DefaultENUM_TESTTRAITS<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMin = T::eSTART;
            template    <typename T, typename SIGNED_DIFF_TYPE, typename UNSIGNED_DIFF_TYPE>
            const T DefaultENUM_TESTTRAITS<T, SIGNED_DIFF_TYPE, UNSIGNED_DIFF_TYPE>::kMax = T::eLAST;
#endif

            /**
             */
            template    <typename T>
            struct  DefaultDiscreteRangeTraits : DefaultRangeTraits<T> {
                // needed for iterator - return false if no more
                static bool GetNext (T* n) {
                    //tmphack
                    *n = static_cast<T> (static_cast<int> (*n) + 1);
                    return true;
                }
            };


            /**
             *  EXAMPLE USAGE:
             *      vector<int> v = DiscreteRange (1,10).As<vector<int>> ();
             *      // equiv to vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
             *
             *  OR:
             *      for (auto i : DiscreteRange (1,10)) {
             *          ...i = 1..10
             *      }
             */
            template    <typename T, typename TRAITS = DefaultDiscreteRangeTraits<T>>
            class  DiscreteRange : public Range<T, TRAITS>, public Iterable<T> {
            private:
#if     !qCompilerAndStdLib_Supports_SharedPtrOfPrivateTypes
            public:
#endif
                struct   MyIteratorRep_;
                struct   MyIteratableRep_;

            public:
                /**
                 */
#if 0
                DiscreteRange ();
#endif
                explicit DiscreteRange (const Memory::Optional<T>& begin, const Memory::Optional<T>& end);

            public:
                /**
                 */
                static  DiscreteRange<T, TRAITS> FullRange ();

            public:
                nonvirtual  bool empty () const;

            public:
                nonvirtual  Iterator<T> begin () const;
                nonvirtual  Iterator<T> end () const;
            };



        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DiscreteRange.inl"

#endif  /*_Stroika_Foundation_Traversal_DiscreteRange_h_ */
