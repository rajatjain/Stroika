/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_ProcessRunner_h_
#define _Stroika_Foundation_Execution_ProcessRunner_h_  1

#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Characters/String.h"
#include	"../Containers/Sequence.h"
#include	"../Memory/Optional.h"
#include	"../Streams/BinaryInputStream.h"
#include	"../Streams/BinaryOutputStream.h"

#include	"IRunnable.h"
#include    "ProgressMonitor.h"


/**
 *  TODO:
 *		@todo	Completely UNIMPLEMETNED
 *				(draft implementation for WINDOWS - based on RFLLib code, but that has a way to go, and nothing for
 *				POSIX yet)
 *
 *		@todo	logic for THREADs and for PROGRESS support are NOT thought through, and just a rough first stab
 *
 *      @todo   Make sure it handles well without blocking
 *              (tricks I had todo in HF - forcing extra reads so writes woudlnt block).
 *				Currently structured to work off a single runnable, which implies works off a single thread. That implies
 *				it must use select() - probably a good idea anyhow - on each socket used for operations (windows and POSIX).
 *
 *				So data pusher/buffer loop does select on external streams to see if data avialble.
 *
 *				This implies I must also be able to do the moral equivilent of selects on my BinaryInput/Output streams? Maybe,
 *				unless I do all the buffering... But at least for the stdin stream - I need to be able to check if/whjen there
 *				is new data available!!! TRICKY
 *
 *		@todo	Decide on/document semantics if there is a change in setting STDIN/SETDOUT etc while a runner exists?
 *				If error - always defectable?
 *
 *				And related - what if we create a runner, and then destroyu the object? How to assure runner fully
 *				destroyed? Blocking/waiting or error or detachted state?
 *
 *		@todo	Add optional hook to be run (at least for posix) inside the FORKED process, before the exec.
 *				Can be used to reset signals, and/or close file descriptors. Maybe have optional
 *				flag to auto-do this stuff and or have a preset value hook proc do do most standard things.
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


			/**
			 *	\brief Synchronously run the given command, and optionally support stdin/stdout/stderr as streams
			 *
			 *	Synchronously run the given command.
			 *

			 // IDEA HERE IS FROM KDJ - Do something like python/perl stuff for managing subprocesses easily.

            // Look input stream, output stream(or streams - stdout/stderr) - and some kind of extenral process control
            // so can say WIAT or Termiante.
            //
            // Simple portable wrapper.
            //
            // Could use simple singly threaded approach used in TypeNValue ReportDefinition::RunExternalProcess_ (const TString& cmdLine, const TString& currentDir, const BLOBs::BLOB& stdinBLOB, const ContentType& resultFormat, float timeout)
            // except that code has the defect that when the input pipe is full, and there is nothing in the output piples
            // it busy waits. We COULD fix this by doing a select.
            //
            // OR - as KDJ suggests - create 3 threads - one that just reads on stdout, one that just reads on stderr, and one that
            // spits into stdin.
            //
            // The caller of 'subprocess' then would just wait on each of the 3 subprocesses (or would implement the aforementioned
            // looping over reads/writes/selects etc).
            //
			 */
			class ProcessRunner {
			public:
				NO_DEFAULT_CONSTRUCTOR (ProcessRunner);
				NO_COPY_CONSTRUCTOR (ProcessRunner);
				NO_ASSIGNMENT_OPERATOR (ProcessRunner);
			public:
				ProcessRunner (const TString& commandLine, Streams::BinaryInputStream in = Streams::BinaryInputStream (), Streams::BinaryOutputStream out = Streams::BinaryOutputStream (), Streams::BinaryOutputStream error = Streams::BinaryOutputStream ());
				ProcessRunner (const TString& executable, const Containers::Sequence<TString>& args, Streams::BinaryInputStream in = Streams::BinaryInputStream (), Streams::BinaryOutputStream out = Streams::BinaryOutputStream (), Streams::BinaryOutputStream error = Streams::BinaryOutputStream ());

			public:
				/**
				 * defaults to CWD at the time the ProcessRunner was created
				 */
				nonvirtual	TString GetWorkingDirectory ();
				nonvirtual	void	SetWorkingDirectory (const TString& d);

			public:
				/**
				 *	If empty, stdin will not be empty (redirected from /dev/null).
				 *
				 *	Otherwise, the stream will be 'read' by the ProcessRunner and 'fed' downstream to
				 *	the running subprocess.
				 */
				nonvirtual	Streams::BinaryInputStream	GetStdIn () const;
				nonvirtual	void						SetStdIn (const Streams::BinaryInputStream& in);
				nonvirtual	void						SetStdIn (const Memory::BLOB& in);

			public:
				/**
				 *	If empty, stdout will not be captured (redirected to /dev/null)
				 */
				nonvirtual	Streams::BinaryOutputStream	GetStdOut () const;
				nonvirtual	void						SetStdOut (const Streams::BinaryOutputStream& out);

			public:
				/**
				 *	If empty, stderr will not be captured (redirected to /dev/null)
				 */
				nonvirtual	Streams::BinaryOutputStream	GetStdErr () const;
				nonvirtual	void						SetStdErr (const Streams::BinaryOutputStream& err);

			public:
				/**
				 *	Note that 'in' will be sent to the stdin of the subprocess, 'out' will be read from the 
				 *	stdout of the subprocess and error will be read from the stderr of the subprocess.
				 *
				 *	Each of these CAN be null, and will if so, that will be interpretted as an empty stream
				 *	(for in/stdin), and for out/error, just means the results will be redirected to /dev/null.
				 */
				nonvirtual	IRunnablePtr	CreateRunnable (ProgressMontior progress = ProgressMontior ());

			public:
				/**
				 *	Creates the runnable above, and directly runs it in place (with the given timeout).
				 *	To be able to control cancelation, use CreateRunnable () directly.
				 */
				nonvirtual	void	Run (ProgressMontior progress = ProgressMontior (), Time::DurationSecondsType timeout = Time::kInfinite);
			
			private:
				Memory::Optional<TString>		fCommandLine_;
				Memory::Optional<TString>		fExecutable_;
				Containers::Sequence<TString>	fArgs_;			// ignored if fExecutable empty
				TString							fWorkingDirectory_;
				Streams::BinaryInputStream		fStdIn_;
				Streams::BinaryOutputStream		fStdOut_;
				Streams::BinaryOutputStream		fStdErr_;
			};


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_ProcessRunner_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ProcessRunner.inl"
