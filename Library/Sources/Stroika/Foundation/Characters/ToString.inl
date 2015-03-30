/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_inl_
#define _Stroika_Foundation_Characters_ToString_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Characters {


            /*
             ********************************************************************************
             ********************************* ToString *************************************
             ********************************************************************************
             */
            template    <typename T>
            String  ToString (const T& t)
            {
                return t.ToString ();
            }


            // Predefined conversions
            template    <>
            inline  String  ToString (const String& t)
            {
                return t;
            }

#if 0
            /*
             * From section from section 3.9.1 of http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3337.pdf
             *      There are five standard signed integer types : signed char, short int, int,
             *      long int, and long long int. In this list, each type provides at least as much
             *      storage as those preceding it in the list.
             *      For each of the standard signed integer types, there exists a corresponding (but different)
             *      standard unsigned integer type: unsigned char, unsigned short int, unsigned int, unsigned long int,
             *      and unsigned long long int, each of which occupies the same amount of storage and has the
             *      same alignment requirements.
             */
#endif
            template    <>
            String  ToString (const int& t);


        }
    }
}
#endif  /*_Stroika_Foundation_Characters_ToString_inl_*/
