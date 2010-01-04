/* -*-c++-*- */
/*
 * A class to fork and exec processes.
 *
 * Copyright 2009, William Matthew Limited.
 *
 * Author: S James <sjames@wmltd.co.uk>
 */

#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
extern "C" {
#include <sys/poll.h>
}
#define PROCESS_MAIN_APP 0
#define PROCESS_FAILURE -1

// Possible errors to be generated
#define PROCESSNOERROR       0
#define PROCESSFAILEDTOSTART 1
#define PROCESSCRASHED       2
#define PROCESSTIMEDOUT      3
#define PROCESSWRITEERR      4
#define PROCESSREADERR       5
#define PROCESSUNKNOWN       6

using namespace std;

namespace wml {

	/*!
	 * A set of virtual callbacks. These should be derived in the
	 * client code. They're called by Process via the
	 * ProcessCallbacks* callbacks member variable.
	 */
	class ProcessCallbacks
	{
	public:
		ProcessCallbacks() {}
		virtual ~ProcessCallbacks() {}
		virtual void startedSignal (string) {}
		virtual void errorSignal (int) {}
		virtual void processFinishedSignal (string) {}
		virtual void readyReadStandardOutputSignal (void) {}
		virtual void readyReadStandardErrorSignal (void) {}
	};

	/*!
	 * Process is a simple replacement for the Qt class QProcess.
	 */
	class Process
	{
	public:
		Process();
		~Process();

		/*!
		 * Write \arg input to the stdin of the process.
		 */
		void writeIn (string& input);

		/*!
		 * fork and exec the process.
		 */
		int start (const string& program, const list<string>& args);

		/*!
		 * Send a TERM signal to the process.
		 */
		void terminate (void);

		/*!
		 * poll to see if there is data on stderr or stdout
		 * and to see if the process has exited.
		 *
		 * This must be called on a scheduled basis. It checks
		 * for any stdout/stderr data and also checks whether
		 * the process is still running.
		 */
		void probeProcess (void);

		/*!
		 * Accessors
		 */
		//@{
		pid_t getPid (void);
		int getError (void);
		void setError (int e);

		/*!
		 * Setter for the callbacks.
		 */
		void setCallbacks (ProcessCallbacks * cb);
		//@}

		/*!
		 * Slots
		 */
		//@{
		string readAllStandardOutput (void);
		string readAllStandardError (void);

		/*!
		 * Wait for the process to get itself going. Do this
		 * by looking at pid.  If no pid after a while,
		 * return false.
		 */
		bool waitForStarted (void);
		//@}
	private:
		/*!
		 * The name of the program to execute
		 */
		string progName;

		/*!
		 * The environment and arguments of the program to execute
		 */
		list<string> environment;

		/*!
		 * Holds a Process error, defined above. PROCESSNOERROR, etc.
		 */
		int error;

		/*!
		 * Process ID of the program
		 */
		pid_t pid;

		/*!
		 * Set to true if the fact that the program has been
		 * started has been signalled using the callback
		 * callbacks->startedSignal
		 */
		bool signalledStart;

		/*!
		 * stdin parent to child
		 */
		int parentToChild[2];

		/*!
		 * stdout child to parent
		 */
		int childToParent[2];

		/*!
		 * stderr child to parent
		 */
		int childErrToParent[2];

		/*!
		 * Used in the poll() call in probeProcess()
		 */
		struct pollfd * p;

		/*!
		 * Pointer to a callback object
		 */
		ProcessCallbacks * callbacks;
	};

} // namespace wml

#endif // _PROCESS_H_
