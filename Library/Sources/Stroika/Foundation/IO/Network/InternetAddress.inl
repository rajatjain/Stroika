/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_InternetAddress_inl_
#define _Stroika_Foundation_IO_Network_InternetAddress_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                /*
                 ********************************************************************************
                 *********************** IO::Network::InternetAddress ***************************
                 ********************************************************************************
                 */
#if     qPlatform_POSIX
                inline  InternetAddress::InternetAddress (const in_addr_t& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ ()
                    , fV6_ ()
                {
                    fV4_.s_addr = i;
                }
#endif
                inline  InternetAddress::InternetAddress (const in_addr& i)
                    : fAddressFamily_ (AddressFamily::V4)
                    , fV4_ (i)
                    , fV6_ ()
                {
                }
                inline  InternetAddress::InternetAddress (const in6_addr& i)
                    : fAddressFamily_ (AddressFamily::V6)
                    , fV4_ ()
                    , fV6_ (i)
                {
                }
                inline  bool    InternetAddress::empty () const
                {
                    return fAddressFamily_ == AddressFamily::UNKNOWN;
                }
                inline  void    InternetAddress::clear ()
                {
                    fAddressFamily_ = AddressFamily::UNKNOWN;
                }
                inline  InternetAddress::AddressFamily  InternetAddress::GetAddressFamily () const
                {
                    return fAddressFamily_;
                }
#if     qPlatform_POSIX
                template    <>
                in_addr_t   InternetAddress::As<in_addr_t> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    return fV4_.s_addr;
                }
#endif
                template    <>
                inline  in_addr InternetAddress::As<in_addr> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V4);
                    return fV4_;
                }
                template    <>
                inline  in6_addr    InternetAddress::As<in6_addr> () const
                {
                    Require (fAddressFamily_ == AddressFamily::V6);
                    return fV6_;
                }


            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_InternetAddress_inl_*/
