/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_inl_
#define _Stroika_Foundation_IO_Network_InternetProtocol_ICMP_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {
                namespace InternetProtocol {
                    namespace ICMP {

                        /*
                         ********************************************************************************
                         ********************** DestinationUnreachableException *************************
                         ********************************************************************************
                         */
                        inline unsigned int DestinationUnreachableException::GetCode () const
                        {
                            return fCode_;
                        }

                        /*
                         ********************************************************************************
                         ***************************** UnknownICMPPacket ********************************
                         ********************************************************************************
                         */
                        inline ICMP_PacketTypes UnknownICMPPacket::GetType () const
                        {
                            return fType_;
                        }
                    }
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_InternetProtocol_ICMP_inl_*/
