/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_Collection_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_Factory_inl_

#include    "Collection_Array.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ********************** Collection_Factory<T, TRAITS> ***************************
                 ********************************************************************************
                 */
                template    <typename T>
                atomic<Collection<T> (*) ()>   Collection_Factory<T>::sFactory_ (nullptr);

                template    <typename T>
                inline  Collection<T>  Collection_Factory<T>::mk ()
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    auto f = sFactory_.load ();
                    if (f == nullptr) {
                        f = &Default_;
                    }
                    return f ();
                }
                template    <typename T>
                void    Collection_Factory<T>::Register (Collection<T> (*factory) ())
                {
                    sFactory_ = factory;
                }
                template    <typename T>
                Collection<T>  Collection_Factory<T>::Default_ ()
                {
                    return Collection_Array<T> ();
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Collection_Factory_inl_ */
