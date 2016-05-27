/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Modbus_Server_inl_
#define _Stroika_Frameworks_Modbus_Server_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Foundation/Debug/Assertions.h"


namespace   Stroika {
    namespace   Frameworks {
        namespace   Modbus {


            /*
             ********************************************************************************
             ***************************** Modbus::SplitSrcAcrossOutputs ********************
             ********************************************************************************
             */
            template    <typename MODBUS_REGISTER_DESCRIPTOR, typename SRC_TYPE>
            void    SplitSrcAcrossOutputs (const SRC_TYPE& s, typename MODBUS_REGISTER_DESCRIPTOR::NameType baseRegister, Containers::Mapping<typename MODBUS_REGISTER_DESCRIPTOR::NameType, typename MODBUS_REGISTER_DESCRIPTOR::ValueType>* update)
            {
                using   VT = typename MODBUS_REGISTER_DESCRIPTOR::ValueType;
                constexpr   size_t  kNElts_  =   sizeof (SRC_TYPE) / sizeof (VT);
                static_assert (kNElts_ * sizeof (VT) == sizeof (SRC_TYPE), ""); // only works if divides evenly
                RequireNotNull (update);
                const VT* arr = reinterpret_cast<const VT*> (&s);
                for (size_t i = 0; i < kNElts_; ++i) {
                    // high order addressed elements first - big endian
                    update->Add (baseRegister + i - 1, arr[kNElts_ -  i - 1]);
                }
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_Modbus_Server_inl_*/
