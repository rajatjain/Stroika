/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedByValue_h_
#define _Stroika_Foundation_Memory_SharedByValue_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"

#include    <memory>


/**
 *  @todo   SharedByValue - even when given an effectively ZERO-SIZED arg for COPIER - since it
 *          saves it as a data member and has min-size 1 byte it wastes space in SharedByValue<T>.
 *          See if this is optimized away in release builds and if not, see if we can find some
 *          clever way around it. (template specializaiton?). Note - this is exactly what the
 *          msvc vector<T> template does so presumably it IS optimized.
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *  \brief  SharedByValue_CopyByFunction is used a template parameter for copying SharedByValue
             *
             *  SharedByValue_CopyByFunction is a simple copying mechanism used by SharedByValue<>.
             *  It is not the most efficient approach (since it stores an actual pointer for the
             *  copy function. But its very simple and usually adequate.
             */
            template    <typename   T, typename SHARED_IMLP = shared_ptr<T> >
            struct  SharedByValue_CopyByFunction {
#if     qCompilerAndStdLib_Supports_lambda_default_argument
                SharedByValue_CopyByFunction (T * (*copier) (const T&) = [](const T& t) { return new T (t); });
#else
                static  T*  DefaultElementCopier_ (const T& t);
                SharedByValue_CopyByFunction (T * (*copier) (const T&) = DefaultElementCopier_);
#endif
                nonvirtual  T*  Copy (const T& t) const;
                T*      (*fCopier) (const T&);
            };


            /**
             *  \brief  SharedByValue_CopyByDefault is the default template parameter for copying SharedByValue
             *
             * SharedByValue_CopyByDefault is the a simple copying mechanism used by SharedByValue<>.
             * It simply hardwires use of new T() - the default T(T&) constructor to copy elements of type T.
             */
            template    <typename   T, typename SHARED_IMLP = shared_ptr<T> >
            struct  SharedByValue_CopyByDefault {
                static  T*  Copy (const T& t);
            };


            /**
             *  \brief  SharedByValue_Traits is a utilitity struct to provide parameterized support
             *          for SharedByValue<>
             *
             *  This class should allow SHARED_IMLP to be std::shared_ptr (or another sharedptr implementation).
             *
             *  Design Note:
             *      We considered re-ordering the template parameters so that SHARED_IMPL came first.
             *      This would be better in some ways becauses the SHARED_IMPL must be passed as a
             *      parameter to SharedByValue_CopyByDefault<>. But since its fairly common to override
             *      the COPIER, and rare to overrided the SHARED_PTR_IMPL (and since we cannot
             *      overload) - we chose this ordering of template parameters.
            */
            template    <typename   T, typename COPIER = SharedByValue_CopyByDefault<T, shared_ptr<T> >, typename SHARED_IMLP = shared_ptr<T> >
            struct   SharedByValue_Traits {
                typedef T               element_type;
                typedef COPIER          copier_type;
                typedef SHARED_IMLP     shared_ptr_type;
            };



            /**
             *  \brief  SharedByValue is a utility class to implement Copy-On-Write.
             *
             *  This utility class should not be used lightly. Its somewhat tricky to use properly. Its meant
             *  to facilitiate implementing the copy-on-write semantics which are often handy in providing
             *  high-performance data structures.
             *
             *  This class should allow SHARED_IMLP to be std::shared_ptr (or another sharedptr implementation).
             *
             *  This class template was originally called CopyOnWrite.
            */
            template    <typename TRAITS = SharedByValue_Traits<T> >
            class   SharedByValue {
            public:
                typedef typename TRAITS::element_type           element_type;
                typedef typename TRAITS::copier_type            copier_type;
                typedef typename TRAITS::shared_ptr_type        shared_ptr_type;

            public:
                /**
                 * SharedByValue::SharedByValue():
                 *  The constructor takes either no/args to nullptr, to construct an empty SharedByValue.
                 *
                 *  It can be copied by another copy of the same kind (including same kind of copier).
                 *
                 *  Or it can be explicitly constructed from a SHARED_IMPL (any existing shared_ptr, along
                 *  with a copier (defaults to SharedByValue_CopyByDefault). If passed a bare pointer, that
                 *  pointer will be wrapped in a shared_ptr (so it better not be already), and the SharedByValue()
                 *  will take ownership of the lifetime of that pointer.
                 */
                SharedByValue ();
                SharedByValue (nullptr_t n);
                SharedByValue (const SharedByValue<TRAITS>& from);
                explicit SharedByValue (const shared_ptr_type& from, const copier_type& copier = copier_type ());
                explicit SharedByValue (element_type* from, const copier_type& copier = copier_type ());

            public:
                nonvirtual  SharedByValue<TRAITS>& operator= (const SharedByValue<TRAITS>& src);

            public:
                /**
                 * get () returns the real underlying ptr we store. It can be nullptr. This should
                 * rarely be used - use operator-> in preference. This is only for dealing with cases where
                 * the ptr could legitimately be nil.
                 */
                nonvirtual  const element_type*    get () const;
                nonvirtual  element_type*          get ();

            public:
                /*
                 * These operators require that the underlying ptr is non-nil.
                 */
                nonvirtual  const element_type*    operator-> () const;
                nonvirtual  element_type*          operator-> ();
                nonvirtual  const element_type&    operator* () const;
                nonvirtual  element_type&          operator* ();


            public:
                /**
                 */
                nonvirtual  bool operator== (const SharedByValue<TRAITS>& rhs) const;

            public:
                /**
                 */
                nonvirtual  bool operator!= (const SharedByValue<TRAITS>& rhs) const;

            public:
                /**
                 * Returns true if there is exactly one object referenced. Note that if empty () - then not unique().
                 */
                nonvirtual  bool    unique () const;

            private:
                shared_ptr_type fSharedImpl_;
                copier_type     fCopier_;

            public:
                /**
                 * Assure there is a single reference to this object, and if there are more, break references.
                 * This method should be applied before destructive operations are applied to the shared object.
                 */
                nonvirtual  void    Assure1Reference ();

            private:
                nonvirtual  void    BreakReferences_ ();
            };

        }
    }
}
#endif  /*_Stroika_Foundation_Memory_SharedByValue_h_*/







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SharedByValue.inl"
