/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DenseDataHyperRectangle_inl_
#define _Stroika_Foundation_Containers_DenseDataHyperRectangle_inl_

#include "Factory/DenseDataHyperRectangle_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ******************** DenseDataHyperRectangle<T, INDEXES...> ********************
             ********************************************************************************
             */
            template <typename T, typename... INDEXES>
            DenseDataHyperRectangle<T, INDEXES...>::DenseDataHyperRectangle (INDEXES... dimensions)
                : inherited (move (Factory::DenseDataHyperRectangle_Factory<T, INDEXES...>::mk (std::forward<INDEXES> (dimensions)...)))
            {
            }
            template <typename T, typename... INDEXES>
            inline DenseDataHyperRectangle<T, INDEXES...>::DenseDataHyperRectangle (const DenseDataHyperRectangle<T, INDEXES...>& src)
                : inherited (static_cast<const inherited&> (src))
            {
            }
            template <typename T, typename... INDEXES>
            inline DenseDataHyperRectangle<T, INDEXES...>::DenseDataHyperRectangle (const _SharedPtrIRep& src) noexcept
                : inherited ((RequireNotNull (src), src))
            {
                this->_AssertRepValidType ();
            }
            template <typename T, typename... INDEXES>
            inline DenseDataHyperRectangle<T, INDEXES...>::DenseDataHyperRectangle (_SharedPtrIRep&& src) noexcept
                : inherited ((RequireNotNull (src), move (src)))
            {
                this->_AssertRepValidType ();
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_DenseDataHyperRectangle_inl_ */