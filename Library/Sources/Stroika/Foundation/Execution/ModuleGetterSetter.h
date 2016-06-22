/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ModuleGetterSetter_h_
#define _Stroika_Foundation_Execution_ModuleGetterSetter_h_ 1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/Common.h"



/**
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * TODO
 *
 *      @todo   NEEDS EXAMPLE - better docs
 *      @todo   cleanup operaotr-> crap - really sloppy impl!!!!
 *      @todo   Consider addiing Update method (with func arg taking T by reference?/optr or taking and returing
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  \breif  Helper to define synchonized, lazy constructed, module initialization (intended to work with DataExchange::OptionFile)
             *
             * Features:
             *      o   Simple API - get/set
             *      o   auto intrindically threadsafe
             *      o   Init underling object on first access, so easy to declare globally (static init) and less worry about running before main
             *      o   IMPL need not worry about thread safety. Just init on CTOR, and implement Get/Set methods.
             *
             *  \par Example Usage
             *      \code
             *          struct  MyData_ {
             *              bool        fEnabled = false;
             *              DateTime    fLastSynchronizedAt;
             *          };
             *          struct  ModuleGetterSetter_Implementation_MyData_ {
             *              ModuleGetterSetter_Implementation_MyData_ ()
             *                  : fOptionsFile_ {
             *                      L"MyModule",
             *                      [] () -> ObjectVariantMapper {
             *                          ObjectVariantMapper mapper;
             *                          mapper.AddClass<MyData_> ({
             *                              ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fEnabled), L"Enabled" },
             *                              ObjectVariantMapper::StructFieldInfo { Stroika_Foundation_DataExchange_StructFieldMetaInfo (MyData_, fLastSynchronizedAt), L"Last-Synchronized-At" },
             *                          });
             *                          return mapper;
             *                      } ()
             *                  }
             *                  , fActualCurrentConfigData_ (fOptionsFile_.Read<MyData_> (MyData_ ()))
             *                  {
             *                      Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
             *                  }
             *                  MyData_   Get ()
             *                  {
             *                      return fActualCurrentConfigData_;
             *                  }
             *                  void    Set (const MyData_& v)
             *                  {
             *                      fActualCurrentConfigData_ = v;
             *                      fOptionsFile_.Write (v);
             *                  }
             *              private:
             *                  OptionsFile     fOptionsFile_;
             *                  MyData_         fActualCurrentConfigData_;      // automatically initialized just in time, and externally synchonized
             *          };
             *
             *          using   Execution::ModuleGetterSetter;
             *          ModuleGetterSetter<MyData_, ModuleGetterSetter_Implementation_MyData_>  sModuleConfiguration_;
             *
             *          void    TestUse_ ()
             *          {
             *              if (sModuleConfiguration_.Get ().fEnabled) {
             *                  auto n = sModuleConfiguration_.Get ();
             *                  sModuleConfiguration_.Set (n);
             *              }
             *          }
             *      \endcode
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             *

             */
            template    <typename T, typename IMPL>
            struct  ModuleGetterSetter {
                /**
                 */
                nonvirtual  T       Get ();

                /**
                 */
                nonvirtual  void    Set (const T& v);

            private:
                Execution::Synchronized<Memory::Optional<IMPL>>   fIndirect_;

            private:
                // Force IMPL CTOR out of line
                nonvirtual  void    DoInitOutOfLine_ (typename Execution::Synchronized<Memory::Optional<IMPL>>::WritableReference* ref);
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ModuleGetterSetter.inl"

#endif  /*_Stroika_Foundation_Execution_ModuleGetterSetter_h_*/
