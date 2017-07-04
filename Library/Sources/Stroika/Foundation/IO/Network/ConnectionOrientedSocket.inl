/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedSocket_inl_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedSocket_inl_ 1

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
                 ************ Foundation::IO::Network::ConnectionOrientedSocket *****************
                 ********************************************************************************
                 */
                inline shared_ptr<ConnectionOrientedSocket::Ptr::_IRep> ConnectionOrientedSocket::_GetSharedRep () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return dynamic_pointer_cast<ConnectionOrientedSocket::_IRep> (inherited::_GetSharedRep ());
                }
                inline ConnectionOrientedSocket::_IRep& ConnectionOrientedSocket::_ref () const
                {
                    AssertMember (&inherited::_ref (), _IRep);
                    return *reinterpret_cast<_IRep*> (&inherited::_ref ());
                }
                inline const ConnectionOrientedSocket::_IRep& ConnectionOrientedSocket::_cref () const
                {
                    AssertMember (&inherited::_cref (), _IRep);
                    return *reinterpret_cast<const _IRep*> (&inherited::_cref ());
                }

                /*
                 ********************************************************************************
                 ************ Foundation::IO::Network::ConnectionOrientedSocket::Ptr ************
                 ********************************************************************************
                 */
                inline ConnectionOrientedSocket::Ptr::Ptr (nullptr_t)
                    : inherited (shared_ptr<inherited::_IRep>{})
                {
                }
                inline ConnectionOrientedSocket::Ptr::Ptr (const ConnectionOrientedSocket& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedSocket::Ptr::Ptr (const Ptr& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedSocket::Ptr::Ptr (Ptr&& src)
                    : inherited (src._GetSharedRep ())
                {
                }
                inline ConnectionOrientedSocket::Ptr& ConnectionOrientedSocket::Ptr::operator= (const Ptr& rhs)
                {
                    Socket::Ptr::operator= (rhs);
                    return *this;
                }
                inline ConnectionOrientedSocket::Ptr& ConnectionOrientedSocket::Ptr::operator= (Ptr&& rhs)
                {
                    Socket::Ptr::operator= (move (rhs));
                    return *this;
                }
                inline void ConnectionOrientedSocket::Ptr::Close () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    inherited::Close ();
                }
                inline void ConnectionOrientedSocket::Ptr::Connect (const SocketAddress& sockAddr) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _ref ().Connect (sockAddr);
                }
                inline size_t ConnectionOrientedSocket::Ptr::Read (Byte* intoStart, Byte* intoEnd) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _ref ().Read (intoStart, intoEnd);
                }
                inline Memory::Optional<size_t> ConnectionOrientedSocket::Ptr::ReadNonBlocking (Byte* intoStart, Byte* intoEnd) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _ref ().ReadNonBlocking (intoStart, intoEnd);
                }
                inline void ConnectionOrientedSocket::Ptr::Write (const Byte* start, const Byte* end) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().Write (start, end);
                }
                inline Optional<IO::Network::SocketAddress> ConnectionOrientedSocket::Ptr::GetPeerAddress () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetPeerAddress ();
                }
                inline Optional<Time::DurationSecondsType> ConnectionOrientedSocket::Ptr::GetAutomaticTCPDisconnectOnClose () const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetAutomaticTCPDisconnectOnClose ();
                }
                inline void ConnectionOrientedSocket::Ptr::SetAutomaticTCPDisconnectOnClose (const Optional<Time::DurationSecondsType>& linger) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().SetAutomaticTCPDisconnectOnClose (linger);
                }
                inline auto ConnectionOrientedSocket::Ptr::GetKeepAlives () const -> KeepAliveOptions
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    return _cref ().GetKeepAlives ();
                }
                inline void ConnectionOrientedSocket::Ptr::SetKeepAlives (const KeepAliveOptions& keepAliveOptions) const
                {
                    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                    _ref ().SetKeepAlives (keepAliveOptions);
                }
            }
        }
    }
}
#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedSocket_inl_*/
