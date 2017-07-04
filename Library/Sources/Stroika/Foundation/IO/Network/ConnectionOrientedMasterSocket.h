/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_
#define _Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_ 1

#include "../../StroikaPreComp.h"

#include "ConnectionOrientedSocket.h"

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /**
                 *  This class is to be used with ConnectionOrientedSocket. You create a ConnectionOrientedMasterSocket, and
                 *  Bind () it, and Listen () on it, and the resulting sockets (from Accept()) are of type ConnectionOrientedSocket.
                 *
                 *  \note Since ConnectionOrientedMasterSocket is a smart pointer, the constness of the methods depends on whether they modify the smart pointer itself, not
                 *        the underlying thread object.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
                 */
                class ConnectionOrientedMasterSocket : public Socket::Ptr {
                private:
                    using inherited = Socket::Ptr;

                protected:
                    class _IRep;

                public:
                    class Ptr;

                public:
                    /**
                     *  \par Example Usage
                     *      \code
                     *          ConnectionOrientedMasterSocket ms { Socket::INET, Socket::STREAM };
                     *          ms.Bind (addr);
                     *          ms.Listen (backlog);
                     *      \endcode
                     *
                     *  \note unless you call @Detatch() - socket is CLOSED in DTOR of rep, so when final reference goes away
                     *
                     *  \note ConnectionOrientedMasterSocket is not copyable, but it can be copied into a ConnectionOrientedMasterSocket::Ptr or
                     *        Socket::Ptr.  This is critical to save them in a container, for example.
                     */
                    ConnectionOrientedMasterSocket ()                                        = delete;
                    ConnectionOrientedMasterSocket (ConnectionOrientedMasterSocket&& s)      = delete;
                    ConnectionOrientedMasterSocket (const ConnectionOrientedMasterSocket& s) = delete;
                    ConnectionOrientedMasterSocket (SocketAddress::FamilyType family, Type socketKind, const Optional<IPPROTO>& protocol = {});

                private:
                    ConnectionOrientedMasterSocket (const shared_ptr<_IRep>& rep);

                public:
                    /**
                     *  For copyability, use ConnectionOrientedMasterSocket::Ptr for assigned-to type.
                     */
                    nonvirtual ConnectionOrientedMasterSocket& operator= (ConnectionOrientedMasterSocket&& s) = delete;
                    nonvirtual ConnectionOrientedMasterSocket& operator= (const ConnectionOrientedMasterSocket& s) = delete;

                public:
                    /**
                     *  This function associates a Platform native socket handle with a Stroika wrapper object.
                     *
                     *  Once a PlatformNativeHandle is attached to Socket object, it will be automatically closed
                     *  when the last reference to the socket disappears (or when someone calls close).
                     *
                     *  To prevent that behavior, you can Detatch the PlatformNativeHandle before destroying
                     *  the associated Socket object.
                     */
                    static ConnectionOrientedMasterSocket::Ptr Attach (PlatformNativeHandle sd);

                protected:
                    /**
                     */
                    nonvirtual shared_ptr<_IRep> _GetSharedRep () const;

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual _IRep& _ref () const;

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual const _IRep& _cref () const;
                };

                /**
                 *  \par Example Usage
                 *      \code
                 *          ConnectionOrientedMasterSocket::Ptr ms  = ConnectionOrientedMasterSocket (Socket::INET, Socket::STREAM);
                 *          ms.Bind (addr);
                 *          ms.Listen (backlog);
                 *          Sequence<ConnectionOrientedMasterSocket::Ptr>   l;  // cannot do Sequence<ConnectionOrientedMasterSocket> cuz not copyable
                 *          l.push_back (ms);
                 *      \endcode
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
                 */
                class ConnectionOrientedMasterSocket::Ptr : public ConnectionOrientedMasterSocket {
                private:
                    using inherited = ConnectionOrientedMasterSocket;

                public:
                    /**
                     *  defaults to null (empty ())
                     */
                    Ptr () = default;
                    Ptr (nullptr_t);
                    Ptr (const Ptr& src);
                    Ptr (Ptr&& src);
                    Ptr (const ConnectionOrientedMasterSocket& src);

                public:
                    /**
                    */
                    nonvirtual Ptr& operator= (const Ptr& rhs);
                    nonvirtual Ptr& operator= (Ptr&& rhs);

                public:
                    /**
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     *   throws on error, and otherwise means should call accept
                     */
                    nonvirtual void Listen (unsigned int backlog) const;

                public:
                    /**
                     *  After Listen() on a connected socket returns (not throws) - you can call Accept() on tha same
                     *  socket to allocate a NEW socket with the new connection stream.
                     *
                     *  @todo   Need timeout on this API? Or global (for instance) timeout?
                     *
                     *  \note ***Cancelation Point***
                     */
                    nonvirtual ConnectionOrientedSocket::Ptr Accept () const;
                };

                /**
                 */
                class ConnectionOrientedMasterSocket::_IRep : public Socket::Ptr::_IRep {
                public:
                    virtual ~_IRep ()                               = default;
                    virtual void Listen (unsigned int backlog)      = 0;
                    virtual ConnectionOrientedSocket::Ptr Accept () = 0;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ConnectionOrientedMasterSocket.inl"

#endif /*_Stroika_Foundation_IO_Network_ConnectionOrientedMasterSocket_h_*/
