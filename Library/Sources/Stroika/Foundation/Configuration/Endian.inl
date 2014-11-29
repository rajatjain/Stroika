/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Endian_inl_
#define _Stroika_Foundation_Configuration_Endian_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /*
             ********************************************************************************
             ***************************** Configuration::GetEndianness *********************
             ********************************************************************************
             */
            inline  constexpr   Endian  GetEndianness ()
            {
                union Mix {
                    int sdat;
                    char cdat[4];
                };
                static constexpr Mix mix { 0x1 };
                return mix.cdat[0] == 1 ? Endian::eLittle : Endian::eBig;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Endian_inl_*/
