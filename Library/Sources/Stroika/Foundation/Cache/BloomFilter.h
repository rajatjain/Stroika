/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cache_BloomFilter_h_
#define _Stroika_Foundation_Cache_BloomFilter_h_   1


/*
 *
 * Description:
 *      http://en.wikipedia.org/wiki/Bloom_filter

 * TODO:
 *
 * Notes:
 *
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"





namespace   Stroika {
    namespace   Foundation {
        namespace   Cache {


            // very rough initial draft of API
            template    <typename T>
            class   BloomFilter<T> {
            public:
                void Add (T elt);

            public:
                void Remove (T elt);

            public:
                //  False positive retrieval results are possible, but false negatives are not;
                void IsPresent (T elt) const;
            };


        }
    }
}


#endif  /*_Stroika_Foundation_Cache_BloomFilter_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "Bag.inl"

