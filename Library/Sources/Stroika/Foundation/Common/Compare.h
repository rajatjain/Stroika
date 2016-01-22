/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Compare_h_
#define _Stroika_Foundation_Common_Compare_h_ 1

#include    "../StroikaPreComp.h"

#include    <functional>
#include    <memory>

#include    "../Configuration/Common.h"
#include    "../Configuration/Concepts.h"
#include    "../Configuration/TypeHints.h"



/**
 * TODO:
 *
 *      @todo   REPLACE
 *                  Has_Operator_LessThan<T>::value
 *              with:
 *                  Configuration::LessThanComparable<T> ()
 *              BUT - CANNOT DO until we have vc++ fixed for constexpr functions
 *
 *      @todo   use http://en.cppreference.com/w/cpp/types/enable_if to make this work better
 *              So for example - we want something like:
 *                  enable_if<is_integral<T>> int compare (T, T) { return v1 - v2; }
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Common {


            namespace Private_ {
                // @DEPRECATED!!!!!
                namespace Has_Operator_LessThan_Helper_ {
                    /*
                     *  This trick is based on code from
                     *      http://stackoverflow.com/questions/5768511/using-sfinae-to-check-for-global-operator
                     */
                    using       no  =   char;
                    using       yes =   char[2];
                    struct any_t {
                        template<typename T> any_t (T const&);
                    };
                    no operator< (any_t const&, any_t const&);
                    yes& test (bool);
                    no test (no);
                    template<typename T>
                    struct Has_Operator_LessThan_ {
                        static T const& s;
                        static T const& t;
                        static bool const value = sizeof(test (s < t)) == sizeof(yes);
                    };
                }
                namespace Has_Compare_Helper_ {
                    using namespace Configuration;
                    template <typename T>
                    struct  get_Compare_result {
                    private:
                        template    <typename X>
                        static auto check (X const& x) -> decltype (declval<X> ().Compare (declval<X> ()));
                        static substitution_failure check (...);

                    public:
                        using type = decltype (check (std::declval<T> ()));
                    };
                }
            }


            /**
             *  has value member which is true iff 'T' supports the less than operator.
             *
             *  TODO REPLACE
             *          Has_Operator_LessThan<T>::value
             *  with:
             *          Configuration::LessThanComparable<T> ()
             *
             *  BUT - CANNOT DO until we have vc++ fixed for constexpr functions
             */
            _Deprecated_("use has_lt<>")
            template    <typename T>
            struct  Has_Operator_LessThan :
                    Private_::Has_Operator_LessThan_Helper_::Has_Operator_LessThan_<T> {
            };


            /**
             *  has value member which is true iff 'T' supports the less than operator.
             */
            template    <typename T>
            struct  Has_Compare :
                    Configuration::substitution_succeeded<typename Private_::Has_Compare_Helper_::get_Compare_result<T>::type> {
            };

            /**
             *  Utility you can specialize to define how two types are to be compared equality using the defined operator==(T,T).
             */
            template <typename T>
            struct  ComparerWithEqualsOptionally {
                using       ElementType     =   T;

                /**
                 *  Returns true if "v1 == v2" - or more precisely - if Compare (v1, v2) == 0. Users can template specialize to
                 *  replace these, but they must remain consistent.
                 */
                static  constexpr   bool    Equals (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2);
            };


            /**
             *  Utility you can specialize to define how two types are to be compared equality using the defined operator==(T,T).
             *
             *  \note   Generally use DefaultEqualsComparer<> instead of this, as it automatically selects the right way to compare.
             */
            template <typename T>
            struct  ComparerWithEquals : ComparerWithEqualsOptionally<T> {
                RequireConceptAppliesToTypeMemberOfClass(RequireOperatorEquals, T);
#if     !qCompilerAndStdLib_constexpr_Buggy
                static_assert(Configuration::EqualityComparable<T> (), "T must be EqualityComparable");
#endif
            };


            /**
             *  Utility you can specialize to define how two types are to be compared for ordering (and how that fits with equality)
             *  The default implementation only requires you define operator< (T,T) - and it fiugres out other cases from just that.
             *
             *  @todo - writeup !!! NOTE - ASSERTS ComparerWithWellOrder and ComparerWithEquals compatible - namely a < b and b > a iff .... writeup!!!
             */
            template <typename T>
            struct  ComparerWithWellOrder { /*: ComparerWithEquals<T>*/
                using   ElementType     =   T;

#if     !qCompilerAndStdLib_constexpr_Buggy
                static_assert(Configuration::LessThanComparable<T> (), "T must be LessThanComparable");
#endif
                RequireConceptAppliesToTypeMemberOfClass(RequireOperatorLess, T);

                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 */
                static  constexpr   int     Compare (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2);
                static  constexpr   bool    Equals (Configuration::ArgByValueType<T> v1, Configuration::ArgByValueType<T> v2)
                {
#if     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                    return not (v1 < v2 or v2 < v1);
#else
                    bool    result { not (v1 < v2 or v2 < v1) };
                    //Ensure (not Configuration::EqualityComparable<T> () or result == (v1 == v2));  must check more indirectly to avoid compile error when not equality comparable
                    return result;
#endif
                }
            };


            // @todo - TRY CHANGING USE OF ComparerWithEquals so instance copied around. Verify no perofrmance (size/time)
            //          costs!!!!
            //
            ///EXPERIMENTAL - MIMICS API/CONCEPT of ComparerWithEquals<T> - but without requirement on T
            /// BUT - to make it it work - we need all places that use
            // ComparerWithEquals to keep an instance - and we have t to test/verify that has no codesize/object
            // size implications when used with the above default case (ok that this costs)
            template <typename T>
            struct  ComparerWithEquals_XXX  {
                function<bool(T, T)> fEq;

                ComparerWithEquals_XXX (function<bool(T, T)> e)
                    : fEq (e)
                {
                }
                bool    Equals (T v1, T v2)
                {
                    return fEq (v1, v2);
                }
            };


            /**
             *  DefaultEqualsComparer will procduce an Equals() method from a variety of sources automatically:
             *      o   operator==
             *      o   operator< (!(a<b or b<a)
             *
             *  and SOON
             *      existing Equals() function(global?/method of T?)
             *      existing Compares() function(global?/method of T?)
             *      maybe also other variants like operator!=, etc.
             *
             *      @todo kind of a kludge how we implement - (shared_ptr<int> - but void not a valid base class).
             */
            template    < typename T, typename SFINAE = typename conditional <
#if     qCompilerAndStdLib_hasEqualDoesntMatchStrongEnums_Buggy
                              (Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value) or is_enum<T>::value,
#else
                              (Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value),
#endif
                              ComparerWithEquals<T>,
                              typename conditional <
                                  Configuration::has_lt<T>::value and is_convertible<Configuration::lt_result<T>, bool>::value,
                                  ComparerWithWellOrder<T>,
                                  shared_ptr<int>
                                  >::type
                              >::type
                          >
            struct  DefaultEqualsComparer :  SFINAE {
            };


            /**
             *  DefaultEqualsComparerOptionally will procduce an Equals() if possible, but will still compile otherwise. Its just
             *  that attempts to call its Equals() method will fail.
             *
             *      @todo improve this so it has an Equals () method with a static_assert, so we get a clearer message when used.
             *      @todo kind of a kludge how we implement - (shared_ptr<int> - but void not a valid base class).
             */
            template    <typename T>
            struct  DefaultEqualsComparerOptionally : conditional<(Configuration::has_eq<T>::value and is_convertible<Configuration::eq_result<T>, bool>::value) or (Configuration::has_lt<T>::value and is_convertible<Configuration::lt_result<T>, bool>::value), DefaultEqualsComparer<T>, shared_ptr<int>>::type {
                    };


            /**
             *  \par Example Usage
             *      \code
             *        return Common::CompareNormalizer (GetNativeSocket (), rhs.GetNativeSocket ());
             *      \endcode
             */
            template    <typename INTEGERLIKETYPE>
            constexpr   int CompareNormalizer (INTEGERLIKETYPE lhs, INTEGERLIKETYPE rhs)
            {
#if     qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                return (lhs < rhs) ? -1 : ((lhs == rhs) ? 0 : 1);
#else
                //using ST = typename make_signed<INTEGERLIKETYPE>::type;   // could do this and then diff
                //return static_cast<ST> (lhs) - static_cast<ST> (rhs);
                if (lhs < rhs) {
                    return -1;
                }
                else if (lhs == rhs) {
                    return 0;
                }
                else {
                    return 1;
                }
#endif
            }
            // @todo more specializations
            template    <>
            inline  constexpr   int CompareNormalizer (int lhs, int rhs)
            {
                return lhs - rhs;
            }


            namespace   STL {
                /**
                 *  An STL-compatible variant of 'less' which leverages a stroika comparer. This can be used to use
                 *  Stroika comparares (almost directly) with std STL classes such as map<>, and set<>.
                 */
                template<typename T, typename STROIKA_COMPARER>
                struct less : public binary_function<T, T, bool> {
                    nonvirtual  bool operator() (Configuration::ArgByValueType<T> _Left, Configuration::ArgByValueType<T> _Right) const;
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
#include    "Compare.inl"

#endif  /*_Stroika_Foundation_Common_Compare_h_*/
