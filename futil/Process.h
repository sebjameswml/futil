/* -*-c++-*- */
/*
 * \file Process.h
 *
 * \brief A class for execing processes
 *
 * Process is part of WML futil - it's a class to fork and exec
 * processes.
 *
 *  WML futil is Copyright William Matthew Ltd. 2009.
 *
 *  Author: Seb James <sjames@wmltd.co.uk>
 *
 *  WML futil is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WML futil is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with WML futil.  If not, see <http://www.gnu.org/licenses/>.
 *
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
#define PROCESSNOMOREPIPES   7
#define PROCESSFORKFAILED    8


using namespace std;

namespace wml {

        /*!
         * \brief A set of virtual callbacks for use with the Process
         * class.
         *
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
         * \brief A C++ class to exec processes without use of
         * system().
         *
         * Process is a simple replacement for the Qt class QProcess.
         */
        class Process
        {
        public:
                /*!
                 * Just one constructor. Allocates memory for pollfd*
                 * p.
                 */
                Process();

                /*!
                 * Destructor. Deallocates memory for pollfd* p
                 */
                ~Process();

                /*!
                 * Reset the process ready to be used again. If this
                 * process is still running, return false and don't do
                 * the reset. Otherwise, reset member attributes and
                 * return true.
                 */
                bool reset (void);

                /*!
                 * Write \arg input to the stdin of the process.
                 */
                void writeIn (string& input);

                /*!
                 * When Process::start() is called, pause useconds
                 * before forking and exec-ing the program. This is a
                 * setter for this->pauseBeforeStart.
                 */
                void setPauseBeforeStart (unsigned int useconds);

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
                 * If the process is running - if pid > 0, return
                 * true. Otherwise return false.
                 */
                bool running (void);

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
                 * Number of micro seconds to pause (via a usleep()
                 * call) before forking and execing the program
                 * following the call to Process::start().
                 */
                unsigned int pauseBeforeStart;

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


        /*!
         * \brief A class used as a parent to a callback object.
         *
         * This class is used as a callback object parent when a
         * process is used within a static function, and as such no
         * parent object exists.
         *
         * Example of this usage can be found in WmlnetapuiUtilities.cpp
         */
        class ProcessData
        {
        public:
                /* \name Constructor and Destructor */
                ProcessData (void);
                ~ProcessData (void);

                /* \name Public Class Functions */
                //@{
                /*!
                 * \brief Set the process finished message for a process
                 */
                void setProcessFinishedMsg (std::string message);

                /*!
                 * \brief Set the error num for a process
                 */
                void setErrorNum (int err);

                /*!
                 * \brief Set stdOutReady
                 */
                void setStdOutReady (bool ready);

                /*!
                 * \brief Set stdErrReady
                 */
                void setStdErrReady (bool ready);

                /*!
                 * \brief Getters
                 */
                std::string getProcessFinishedMsg (void);
                int getErrorNum (void);
                bool getStdOutReady (void);
                bool getStdErrReady (void);
                //@}
        private:
                /* \name Private Class Attributes */
                //@{
                /*!
                 * \brief Holds the name of the process that finished
                 */
                std::string processFinishedMessage;

                /*!
                 * \brief Holds a process error number
                 */
                int errorNum;

                /*!
                 * \brief determines if std out is ready to be read from
                 */
                bool stdOutReady;

                /*!
                 * \brief determines if std err is ready to be read from
                 */
                bool stdErrReady;
                //@}
        };

} // namespace wml

#endif // _PROCESS_H_
