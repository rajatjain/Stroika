/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#ifndef _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Factory_inl_
#define _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Factory_inl_

#include "../Concrete/DataHyperRectangle_DenseVector.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /*
                 ********************************************************************************
                 ************************ DataHyperRectangle_Factory<T> *************************
                 ********************************************************************************
                 */
                template <typename T, typename... INDEXES>
                atomic<DataHyperRectangle<T, INDEXES...> (*) (INDEXES...)> DataHyperRectangle_Factory<T, INDEXES...>::sFactory_ (nullptr);
                template <typename T, typename... INDEXES>
                inline DataHyperRectangle<T, INDEXES...> DataHyperRectangle_Factory<T, INDEXES...>::mk (INDEXES... dimensions)
                {
                    /*
                     *  Would have been more performant to just and assure always properly set, but to initialize
                     *  sFactory_ with a value other than nullptr requires waiting until after main() - so causes problems
                     *  with containers constructed before main.
                     *
                     *  This works more generally (and with hopefully modest enough performance impact).
                     */
                    if (auto f = sFactory_.load ()) {
                        return f (std::forward<INDEXES> (dimensions)...);
                    }
                    else {
                        return Default_ (std::forward<INDEXES> (dimensions)...);
                    }
                }
                template <typename T, typename... INDEXES>
                void DataHyperRectangle_Factory<T, INDEXES...>::Register (DataHyperRectangle<T, INDEXES...> (*factory) (INDEXES...))
                {
                    sFactory_ = factory;
                }
                template <typename T, typename... INDEXES>
                inline DataHyperRectangle<T, INDEXES...> DataHyperRectangle_Factory<T, INDEXES...>::Default_ (INDEXES... dimensions)
                {
                    return DataHyperRectangle_DenseVector<T, INDEXES...> (std::forward<INDEXES> (dimensions)...);
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_Factory_inl_ */
