/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Socket_h_
#define _Stroika_Foundation_IO_Network_Socket_h_ 1

#include "../../StroikaPreComp.h"

#if qPlatform_POSIX
#include <sys/socket.h>
#endif

#include "../../Characters/String.h"
#include "../../Configuration/Common.h"
#include "../../Execution/ErrNoException.h"
#if qPlatform_Windows
#include "Platform/Windows/WinSock.h"
#endif
#include "../../Memory/Optional.h"

#include "SocketAddress.h"

#if qPlatform_Linux or qPlatform_MacOS
using IPPROTO = int;
#endif

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-597 - Socket 'smart pointer wrapper' should use Debug::AssertExternallySynchonized (rep already does)  
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                using Characters::String;
                using Memory::Byte;
                using Memory::Optional;

#if qCompilerAndStdLib_deprecated_attribute_itselfProducesWarning_Buggy
                DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
#endif
                /**
                 *  A Socket has two parts, a _IRep*, and a smart pointer wrapper (like shared_ptr <_IRep>).
                 *  Users of the class interact only with the smart pointer wrapper.
                 *
                 *  But for purposes of thread safety, and understanding object lifetime, its important
                 *  to consider both.
                 *
                 *  The Socket class itself is abstract - as you can only create particular kinds of sockets (like connection-oriented etc).
                 *
                 *  And you almost always just interact with and manage the smart pointers - Socket::Ptr.
                 *
                 *  But you construct a concrete subtype and assign it to a pointer. 
                 *
                 *  \par Example Usage
                 *      \code
                 *          Socket::Ptr      s  = ConnectionlessSocket { Socket::INET, Socket::DGRAM };
                 *      \endcode
                 *
                 *  The Socket smart pointer objects can be freely assigned and passed around, but the
                 *  underlying (_IRep*) socket is finally disposed of when the last reference to it
                 *  goes away (or when it is 'Closed').
                 *
                 *  Closing one, closes them all (though overwriting one just has the effect of detatching
                 *  from the underlying socket.
                 *
                 * TODO:
                 *      @todo   In socket class, set CLOSE_ON_EXEC?
                 *
                 *      @todo   Document (or define new expcetion) thrown when operaiton done on CLOSED socket.
                 *              and acutally handle all the nullptr cases...
                 *
                 *      @todo   See about socket 'connected' state, and the 'connect' operation.
                 *              And see about send/recv() API - and docuemnt about only working when
                 *              connected.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
                 */
                class Socket  {
                public:
/**
                    *  Platform Socket descriptor - file descriptor on unix (something like this on windoze)
                    */
#if qPlatform_Windows
                    using PlatformNativeHandle = SOCKET;
#else
                    using PlatformNativeHandle = int;
#endif
                protected:
                    class _IRep;

                public:
                    class Ptr;

                public:
                    /**
                     * 'second arg' to ::socket() call - socket type
                     */
                    enum class Type : int {
                        STREAM = SOCK_STREAM,
                        DGRAM  = SOCK_DGRAM,
                        RAW    = SOCK_RAW,
                    };
                    static constexpr Type STREAM = Type::STREAM;
                    static constexpr Type DGRAM  = Type::DGRAM;
                    static constexpr Type RAW    = Type::RAW;

                protected:
                    /**
                     *  Only Socket::Ptr objects are constructible: Socket is abstract.
                     */
                    Socket ()              = default;
                    Socket (const Socket&) = default;
                    Socket (Socket&&)      = default;
                    Socket (const shared_ptr<_IRep>& src);
                    Socket (shared_ptr<_IRep>&& src);

                public:
                    /**
                    *  Marks this Ptr (and and sockets copied from it, before or after). This can be used
                    *  to prevent the underlying native socket from being closed.
                    *
                    *   @see Close ()
                    */
                    nonvirtual PlatformNativeHandle Detach ();

                public:
                    /**
                    *  Return the first argument to the ::socket() call (address family) or the result of getsockopt (SOL_SOCKET, SO_DOMAIN)
                    */
                    nonvirtual SocketAddress::FamilyType GetAddressFamily () const;

                public:
                    /**
                    *  Return the second argument to the ::socket() call (type) or the result of getsockopt (SOL_SOCKET, SO_TYPE)
                    */
                    nonvirtual Type GetType () const;

                public:
                    /**
                     */
                    struct BindFlags {
                        bool fReUseAddr : 1; // SO_REUSEADDR
                        constexpr BindFlags (bool reUseAddr = false);
                    };

                public:
                    /**
                     *  Associate this socket object with the given address.
                     *
                     *  @todo   SB an overload taking just a port, and defaults to INADDRANY with that port.
                     *          Port is only thing really needed, but InternetAddress part provided as arg
                     *          too in case you want to bind to a particular interface.
                     *
                     *  @todo   CLARIFY if a socket can be bound to more than one address (and what about unbind)?
                     *
                     *  \note   This is usually just used on a ConnectionOrientedMasterSocket but may also be used
                     *          by a ConnectionlessSocket (e.g. with SSDP).
                     *
                     *  @see POSIX bind()
                     */
                    nonvirtual void Bind (const SocketAddress& sockAddr, BindFlags bindFlags = BindFlags ());

                public:
                    /**
                     * if bound (@see Bind ()) - to what local endpoint? Remember a computer can be multi-homed, and can be bound to ADDR_ANY, or a specific address (plus the port).
                     */
                    nonvirtual Optional<IO::Network::SocketAddress> GetLocalAddress () const;

                public:
                    enum class ShutdownTarget {
                        eReads,
                        eWrites,
                        eBoth,

                        eDEFAULT = eBoth,

                        Stroika_Define_Enum_Bounds (eReads, eBoth)
                    };
                    static constexpr ShutdownTarget eReads  = ShutdownTarget::eReads;
                    static constexpr ShutdownTarget eWrites = ShutdownTarget::eWrites;
                    static constexpr ShutdownTarget eBoth   = ShutdownTarget::eBoth;

                public:
                    /**
                     *  @see GetAutomaticTCPDisconnectOnClose - if set - Close automatically calls Shutdown () for connection-oriented sockets.
                     *
                     *  @see https://msdn.microsoft.com/en-us/library/system.net.sockets.socket.shutdown(v=vs.110).aspx
                     *      When using a connection-oriented Socket, always call the Shutdown method before closing the Socket.
                     *      This ensures that all data is sent and received on the connected socket before it is closed
                     *
                     *  @see http://pubs.opengroup.org/onlinepubs/9699919799/
                     *      The shutdown() function shall cause all or part of a full-duplex connection on the socket
                     *      associated with the file descriptor socket to be shut down.
                     *
                     *      The shutdown() function disables subsequent send and/or receive operations on a socket,
                     *      depending on the value of the how argument.
                     *
                     *  @see https://msdn.microsoft.com/en-us/library/windows/desktop/ms740481(v=vs.85).aspx
                     *      If the how parameter is SD_RECEIVE, subsequent calls to the recv function on the socket
                     *      will be disallowed. This has no effect on the lower protocol layers. For TCP sockets,
                     *      if there is still data queued on the socket waiting to be received, or data arrives subsequently,
                     *      the connection is reset, since the data cannot be delivered to the user. For UDP sockets,
                     *      incoming datagrams are accepted and queued. In no case will an ICMP error packet be generated.
                     *
                     *      If the how parameter is SD_SEND, subsequent calls to the send function are disallowed.
                     *      For TCP sockets, a FIN will be sent after all data is sent and acknowledged by the receiver.
                     */
                    nonvirtual void Shutdown (ShutdownTarget shutdownTarget = ShutdownTarget::eDEFAULT);

                public:
                    /**
                     *  Note that Socket is an envelope class, and there could be multiple references to
                     *  the same underlying platform socket. But this closes ALL of them. It also removes the reference
                     *  to the underlying rep (meaning that some Socket envelopes COULD have a rep with an
                     *  underlying closed socket).
                     *
                     *  @see Detach
                     */
                    nonvirtual void Close ();

                public:
                    /**
                     *  A socket can be open or closed. Open is roughly analagous to non-null. A socket once closed
                     *  can never be 'Opened' - but you can assign a new Open socket to the Socket object.
                     *
                     *  \note - not same as ==nullptr? to be open, must not == nullptr and underlying rep must be open.
                     *
                     *  @see Close
                     */
                    nonvirtual bool IsOpen () const;

                public:
                    /**
                     *  Return true iff the sOCKETS are the same.
                     *
                     *  This is like Compare() == 0.
                     *
                     *  \note   Two sockets compare equal iff their underlying native sockets are equal (@see GetNativeSocket)
                     *          This means you can have two Socket objects which compare equal by use of Attach().
                     */
                    nonvirtual bool Equals (const Socket& rhs) const;

                public:
                    /**
                     *
                     *  \note   Sockets are compared by their underlying native sockets (@see GetNativeSocket).
                     *          This means you can have two Socket objects which compare equal by use of Attach().
                     */
                    nonvirtual int Compare (const Socket& rhs) const;

                public:
                    /**
                     *  Return the native platform handle object associated with this socket
                     *  (typically an integer file descriptor)
                     */
                    nonvirtual PlatformNativeHandle GetNativeSocket () const;

                public:
                    /**
                     *  Usually the return value is an int, but the caller must specify the right type. This is a simple,
                     *  low level, wrapper on 'man 2 getsockopt'.
                     *
                     *  \note   this is usually unnecessary, somewhat dangerous (easy to mismatch types), and most common socket options
                     *          can be accessed via other methods (e.g. GetMulticastTTL ())
                     *
                     *  @see setsockopt
                     */
                    template <typename RESULT_TYPE>
                    nonvirtual RESULT_TYPE getsockopt (int level, int optname) const;

                public:
                    /**
                     *  This is a simple, low level, wrapper on 'man 2 setsockopt'.
                     *
                     *  \note   this is usually unnecessary, somewhat dangerous (easy to mismatch types), and most common socket options
                     *          can be accessed via other methods (e.g. SetMulticastTTL ())
                     *
                     *  \note   It maybe advisable to be explicit about the ARG_TYPE in the template, as constants like '3' may get misinterpretted,
                     *          and you must use the exactly correct type for the low level setsockopt API.
                     *
                     *  @see getsockopt
                     */
                    template <typename ARG_TYPE>
                    nonvirtual void setsockopt (int level, int optname, ARG_TYPE arg);

                protected:
                    /**
                     */
                    nonvirtual shared_ptr<_IRep> _GetSharedRep () const;

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual _IRep& _ref ();

                protected:
                    /**
                     * \req fRep_ != nullptr
                     */
                    nonvirtual const _IRep& _cref () const;

                private:
                    shared_ptr<_IRep> fRep_;

                protected:
                    static PlatformNativeHandle mkLowLevelSocket_ (SocketAddress::FamilyType family, Socket::Type socketKind, const Optional<IPPROTO>& protocol);

                public:
                    _Deprecated_ ("USE SocketAddress::FamilyType - deprecated v2.0a207") typedef SocketAddress::FamilyType ProtocolFamily;
                    _Deprecated_ ("USE SocketAddress::INET - deprecated v2.0a207") static constexpr SocketAddress::FamilyType INET   = SocketAddress::INET;
                    _Deprecated_ ("USE SocketAddress::INET6 - deprecated v2.0a207") static constexpr SocketAddress::FamilyType INET6 = SocketAddress::INET6;

                public:
                    // Deprecated - v2.0a206
                    _Deprecated_ ("USE Type") typedef Type SocketKind;
                };
#if qCompilerAndStdLib_deprecated_attribute_itselfProducesWarning_Buggy
                DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
#endif

                /**
                 *  \brief a smart pointer wrapper (like shared_ptr <_IRep>).
                 *
                 *  Users of the class interact only with the smart pointer wrapper.
                 *
                 *  But for purposes of thread safety, and understanding object lifetime, its important
                 *  to consider both.
                 *
                 *  And you almost always just interact with and manage the smart pointers - Socket::Ptr.
                 *
                 *  But you construct a concrete subtype and assign it to a pointer. 
                 *
                 *  \par Example Usage
                 *      \code
                 *          Socket::Ptr      s  = ConnectionlessSocket { Socket::INET, Socket::DGRAM };
                 *      \endcode
                 *
                 *  \par Example Usage
                 *      \code
                 *          Socket::Ptr      s;
                 *          if (s == nullptr) {
                 *              s = ConnectionlessSocket { Socket::INET, Socket::DGRAM };
                 *          }
                 *      \endcode
                 *
                 *  The Socket smart pointer objects can be freely assigned and passed around, but the
                 *  underlying (_IRep*) socket is finally disposed of when the last reference to it
                 *  goes away (or when it is 'Closed').
                 *
                 *  Closing one, closes them all (though overwriting one just has the effect of detatching
                 *  from the underlying socket.
                 *
                 *  \note   select: Socket has no select method: instead use Execution::WaitForIOReady which
                 *          works transparently with sockets, sets of sockets, or other waitable objects.
                 *
                 *  \note   See coding conventions document about operator usage: Compare () and operator<, operator>, etc
                 *
                 *  \note inherits from Socket just for inherited type definitions - no methods or data.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-May-Need-To-Externally-Synchronize-Letter</a>
                 */
                class Socket::Ptr : public Socket {
                private:
                    using inherited = Socket;

                protected:
                    /**
                     *  Socket is now an abstract class. Use an explicit subclass like
                     *      o   ConnectionlessSocket
                     *      o   ConnectionOrientedSocket
                     *      o   ConnectionOrientedMasterSocket
                     *
                     * \note unless you call @Detatch() - socket is CLOSED in DTOR of rep, so when final reference goes away
                     */
                public:
                    Ptr (const Ptr& s) = default;
                    Ptr (Ptr&& s);

                protected:
                    Ptr () = delete;
                    Ptr (shared_ptr<_IRep>&& rep);
                    Ptr (const shared_ptr<_IRep>& rep);

                public:
                    ~Ptr () = default;

                public:
                    /**
                     */
                    nonvirtual Ptr& operator= (Ptr&& s);
                    nonvirtual Ptr& operator= (const Ptr& s);
                };

                /**
                 *  operator indirects to Socket::Ptr::Compare()
                 */
                bool operator< (const Socket::Ptr& lhs, const Socket::Ptr& rhs);

                /**
                 *  operator indirects to Socket::Ptr::Compare()
                 */
                bool operator<= (const Socket::Ptr& lhs, const Socket::Ptr& rhs);

                /**
                 *  operator indirects to Socket::Ptr::Equals()
                 */
                bool operator== (const Socket::Ptr& lhs, const Socket::Ptr& rhs);

                /**
                 *  operator indirects to Socket::Ptr::Equals()
                 */
                bool operator!= (const Socket::Ptr& lhs, const Socket::Ptr& rhs);

                /**
                 *  operator indirects to Socket::Ptr::Compare()
                 */
                bool operator>= (const Socket::Ptr& lhs, const Socket::Ptr& rhs);

                /**
                 *  operator indirects to Socket::Ptr::Compare()
                 */
                bool operator> (const Socket::Ptr& lhs, const Socket::Ptr& rhs);

                /**
                 */
                class Socket::_IRep {
                public:
                    virtual ~_IRep ()                                                      = default;
                    virtual void Shutdown (ShutdownTarget shutdownTarget)                  = 0;
                    virtual void                                 Close ()                  = 0;
                    virtual Optional<IO::Network::SocketAddress> GetLocalAddress () const  = 0;
                    virtual SocketAddress::FamilyType            GetAddressFamily () const = 0;
                    virtual PlatformNativeHandle                 GetNativeSocket () const  = 0;
                    virtual void getsockopt (int level, int optname, void* optval, socklen_t* optvallen) const = 0;
                    virtual void setsockopt (int level, int optname, const void* optval, socklen_t optvallen)  = 0;
                };
            }
        }
    }
}

#if qPlatform_Windows
namespace Stroika {
    namespace Foundation {
        namespace Execution {
            template <>
            IO::Network::Socket::PlatformNativeHandle ThrowErrNoIfNegative (IO::Network::Socket::PlatformNativeHandle returnCode);
        }
    }
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Socket.inl"

#endif /*_Stroika_Foundation_IO_Network_Socket_h_*/
