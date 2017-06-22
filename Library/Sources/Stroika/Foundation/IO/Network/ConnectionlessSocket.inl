/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionlessSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionlessSocket_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /*
                 ********************************************************************************
                 ************** Foundation::IO::Network::ConnectionlessSocket *******************
                 ********************************************************************************
                 */
                inline shared_ptr<ConnectionlessSocket::Ptr::_IRep> ConnectionlessSocket::_GetSharedRep () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return dynamic_pointer_cast<ConnectionlessSocket::_IRep> (inherited::_GetSharedRep ());
                }
                inline ConnectionlessSocket::_IRep& ConnectionlessSocket::_ref ()
                {
                    AssertMember (&inherited::_ref (), _IRep);
                    return *reinterpret_cast<_IRep*> (&inherited::_ref ());
                }
                inline const ConnectionlessSocket::_IRep& ConnectionlessSocket::_cref () const
                {
                    AssertMember (&inherited::_cref (), _IRep);
                    return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
                }
                inline uint8_t ConnectionlessSocket::GetMulticastTTL () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetMulticastTTL ();
                }
                inline void ConnectionlessSocket::SetMulticastTTL (uint8_t ttl)
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().SetMulticastTTL (ttl);
                }
                inline bool ConnectionlessSocket::GetMulticastLoopMode ()
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetMulticastLoopMode ();
                }
                inline void ConnectionlessSocket::SetMulticastLoopMode (bool loopMode)
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().SetMulticastLoopMode (loopMode);
                }
                inline void ConnectionlessSocket::JoinMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().JoinMulticastGroup (iaddr, onInterface);
                }
                inline void ConnectionlessSocket::LeaveMulticastGroup (const InternetAddress& iaddr, const InternetAddress& onInterface)
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().LeaveMulticastGroup (iaddr, onInterface);
                }
                inline void ConnectionlessSocket::SendTo (const Byte* start, const Byte* end, const SocketAddress& sockAddr)
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().SendTo (start, end, sockAddr);
                }
                inline size_t ConnectionlessSocket::ReceiveFrom (Byte* intoStart, Byte* intoEnd, int flag, SocketAddress* fromAddress, Time::DurationSecondsType timeout)
                {
                    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                    return _ref ().ReceiveFrom (intoStart, intoEnd, flag, fromAddress, timeout);
                }

                /*
                 ********************************************************************************
                 ************ Foundation::IO::Network::ConnectionlessSocket::Ptr ****************
                 ********************************************************************************
                 */
                inline ConnectionlessSocket::Ptr::Ptr (nullptr_t)
                    : inherited (shared_ptr<inherited::_IRep>{})
                {
                }
                inline ConnectionlessSocket::Ptr::Ptr (const ConnectionlessSocket& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionlessSocket::Ptr::Ptr (const Ptr& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionlessSocket::Ptr::Ptr (Ptr&& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionlessSocket::Ptr& ConnectionlessSocket::Ptr::operator= (const Ptr& rhs)
                {
                    Socket::Ptr::operator= (rhs);
                    return *this;
                }
                inline ConnectionlessSocket::Ptr& ConnectionlessSocket::Ptr::operator= (Ptr&& rhs)
                {
                    Socket::Ptr::operator= (move (rhs));
                    return *this;
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_ConnectionlessSocket_inl_*/