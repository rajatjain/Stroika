/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleInit_inl_
#define _Stroika_Foundation_Execution_ModuleInit_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

#if 0
            template    <typename T>
            inline  SingletonObjActualInitializer<T>::SingletonObjActualInitializer ()
                : fThe ()
            {
            }
            template    <typename T>
            inline  SingletonObjActualInitializer<T>::~SingletonObjActualInitializer ()
            {
            }
            template    <typename T>
            inline  const T&    SingletonObjActualInitializer<T>::THE () const
            {
                return fThe;
            }
            template    <typename T>
            inline  T&  SingletonObjActualInitializer<T>::THE ()
            {
                return fThe;
            }
#endif






            template    <typename   MODULE_DATA>
            inline  ModuleInitializer<MODULE_DATA>::ModuleInitializer ()
            {
                if (sInitCnt_++ == 0) {
                    // no need to store pointer, cuz its the same as &sActualModuleInitializer_Storage_
                    (void)new (&sActualModuleInitializer_Storage_) MODULE_DATA ();
                }
            }
            template    <typename   MODULE_DATA>
            inline  ModuleInitializer<MODULE_DATA>::~ModuleInitializer ()
            {
                if (--sInitCnt_ == 0) {
                    reinterpret_cast<MODULE_DATA*> (&sActualModuleInitializer_Storage_)->~MODULE_DATA ();
                }
            }
            template    <typename   MODULE_DATA>
            inline  MODULE_DATA&  ModuleInitializer<MODULE_DATA>::Actual ()
            {
                Assert (sInitCnt_ > 0);  // we've been initialized, and not yet destroyed...
                return *reinterpret_cast<MODULE_DATA*> (&sActualModuleInitializer_Storage_);
            }
            template    <typename MODULE_DATA>
            Byte    ModuleInitializer<MODULE_DATA>::sActualModuleInitializer_Storage_[sizeof (MODULE_DATA)]; // avoid actual memory allocation call - since only one of these
            template    <typename MODULE_DATA>
            unsigned    short   ModuleInitializer<MODULE_DATA>::sInitCnt_;


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ModuleInit_inl_*/
