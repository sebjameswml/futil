/*
 * Process is part of WML futil - it's a class to fork and exec
 * processes.
 *
 *  WML futil is Copyright William Matthew Ltd. 2009.
 *
 *  Author: Seb James <sjames@wmltd.co.uk>
 *
 *  WML futil is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WML futil is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WML futil.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <iostream>
#include <sstream>
extern "C" {
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <signal.h>
}

#include "config.h"
#include "WmlDbg.h"
#include "Process.h"

using namespace std;
using namespace wml;

/*!
 * Implementation of the Process class
 */
//@{

// Used when dealing with pipes
#define READING_END 0
#define WRITING_END 1
#define STDIN  0
#define STDOUT 1
#define STDERR 2

// Constructor
wml::Process::Process () :
	progName("unknown"),
	error (PROCESSNOERROR),
	pid(0),
	signalledStart(false)
{
	// Set up the polling structs
	this->p = static_cast<struct pollfd*>(malloc (2*sizeof (struct pollfd)));

	// Initialise the callbacks to 0
	this->callbacks = (ProcessCallbacks*)0;
}

// Destructor
wml::Process::~Process ()
{
	free (this->p);
}

void
wml::Process::writeIn (string& input)
{
	write (this->parentToChild[WRITING_END], input.c_str(), input.size());
}

// fork and exec a new process using execv, which takes stdin via a
// fifo and returns output also via a fifo.
int
wml::Process::start (const string& program, const list<string>& args)
{
	char** argarray;
	list<string> myargs = args;
	list<string>::iterator i;
	unsigned int j = 0;
	int theError;

	// NB: The first item in the args list should be the program name.
	this->progName = program;

	// Set up our pipes
	if (pipe(parentToChild) == -1 || pipe(childToParent) == -1 || pipe(childErrToParent) == -1) {
		return PROCESS_FAILURE;
	}

	this->pid = fork();
	switch (this->pid) {
	case -1:
		return PROCESS_FAILURE;
	case 0:
		// This is the CHILD process

		// Close unwanted ends of the pipes
		close (parentToChild[WRITING_END]);
		close (childToParent[READING_END]);
		close (childErrToParent[READING_END]);

		// Now all we have to do is make the writing file
		// descriptors 0,1 or 2 and they will be used instead
		// of stdout, stderr and stdin.
		if ((dup2 (parentToChild[READING_END], STDIN)) == -1  ||
		    (dup2 (childToParent[WRITING_END], STDOUT)) == -1 ||
		    (dup2 (childErrToParent[WRITING_END], STDERR)) == -1) {
			theError = errno;
			cout << "ERROR! Couldn't get access to stdin/out/err! errno was " << theError << endl;
			return PROCESS_FAILURE;
		}

		// Allocate memory for the program arguments
		// 1+ to allow space for NULL terminating pointer
		argarray = static_cast<char**>(malloc ((1+args.size()) * sizeof (char*)));
		for (i=myargs.begin(); i!=myargs.end(); i++) {
			argarray[j] = static_cast<char*>(malloc ( (1+(*i).size()) * sizeof (char) ));
			snprintf (argarray[j++], 1+(*i).size(), "%s", (*i).c_str());
			DBG (*i);
		}
		argarray[j] = NULL;

		DBG ("About to execute '" + program + "' with those arguments..");

		execv (program.c_str(), argarray);

		// If process returns, error occurred
		theError = errno;
		// This'll get picked up by parseResponse
		cout << "Process error: " << this->pid << " crashed. errno:" << theError << endl;

		// This won't get picked up by the parent process.
		this->error = PROCESSCRASHED;

		// In this case, we close the pipes to signal to the parent that we crashed
		close (parentToChild[READING_END]);
		close (childToParent[WRITING_END]);
		close (childErrToParent[WRITING_END]);

		// Child should exit now.
		_exit(-1);

	default:
		// This is the PARENT process

		// Close unwanted ends of the pipes
		close (parentToChild[READING_END]);
		close (childToParent[WRITING_END]);
		close (childErrToParent[WRITING_END]);

		// Write to this->parentToChild[WRITING_END] to write to stdin of the child
		// Read from this->childToParent[READING_END] to read from stdout of child
		// Read from this->childErrToParent[READING_END] to read from stderr of child

		break;
	}
	return PROCESS_MAIN_APP;
}


// If no pid after a while, return false.
bool
wml::Process::waitForStarted (void)
{
	unsigned int i=0;
	while (this->pid == 0 && i<1000) {
		usleep (1000);
		i++;
	}
	if (this->pid>0) {
		DBG ("The process started!");
		if (this->callbacks != (ProcessCallbacks*)0) {
			this->callbacks->startedSignal (this->progName);
		}
		this->signalledStart = true;
		return true;
	} else {
		this->error = PROCESSFAILEDTOSTART;
		if (this->callbacks != (ProcessCallbacks*)0) {
			this->callbacks->errorSignal (this->error);
		}
		return false;
	}

}

// Send a TERM signal to the process.
void
wml::Process::terminate (void)
{
	kill (this->pid, 15); // 15 is TERM
	// Now check if the process has gone and kill it with signal 9 (KILL)
	this->pid = 0;
	this->error = PROCESSNOERROR;
	this->signalledStart = false;
	return;
}

// Check on this process
void
wml::Process::probeProcess (void)
{
	// Has the process started?
	if (!this->signalledStart) {
		if (this->pid > 0) {
			if (this->callbacks != (ProcessCallbacks*)0) {
				this->callbacks->startedSignal (this->progName);
			}
			this->signalledStart = true;
			DBG ("Process::probeProcess set signalledStart and signalled the start...");
		}
	}

	// Check for error condition
	if (this->error>0) {
		if (this->callbacks != (ProcessCallbacks*)0) {
			this->callbacks->errorSignal (this->error);
		}
		DBG ("have error in probeProcess, returning");
		return;
	}

	if (this->pid == 0) {
		// Not yet started.
		return;
	}

	// Why can't these 4 lines go in contructor?
	this->p[0].fd = this->childToParent[READING_END];
	this->p[0].events = POLLIN | POLLPRI;
	this->p[1].fd = this->childErrToParent[READING_END];
	this->p[1].events = POLLIN | POLLPRI;

	// Poll to determine if data is available
	this->p[0].revents = 0;
	this->p[1].revents = 0;

	poll (this->p, 2, 0);

	if (this->p[0].revents & POLLNVAL || this->p[1].revents & POLLNVAL) {
		DBG ("Process::probeProcess: pipes closed, process must have crashed");
		this->error = PROCESSCRASHED;
		if (this->callbacks != (ProcessCallbacks*)0) {
			this->callbacks->errorSignal (this->error);
		}
		return;
	}

	if (this->p[0].revents & POLLIN || this->p[0].revents & POLLPRI) {
		if (this->callbacks != (ProcessCallbacks*)0) {
			this->callbacks->readyReadStandardOutputSignal();
		}
	}
	if (this->p[1].revents & POLLIN || this->p[1].revents & POLLPRI) {
		if (this->callbacks != (ProcessCallbacks*)0) {
			this->callbacks->readyReadStandardErrorSignal();
		}
	}

	// Is the process running? We check last, so that we get any
	// messages on stdout/stderr that we may wish to process, such
	// as error messages from nxssh key authentication.
	int theError;
	if (this->signalledStart == true) {
		int rtn = 0;
		if ((rtn = waitpid (this->pid, (int *)0, WNOHANG)) == this->pid) {
			if (this->callbacks != (ProcessCallbacks*)0) {
				this->callbacks->processFinishedSignal (this->progName);
			}
			this->pid = 0;
			return;
		} else if (rtn == -1) {
			theError = errno;
			if (theError != 10) { // We ignore errno 10 "no child" as this commonly occurs
				cerr << "waitpid returned errno: " << theError;
			}
		} // else rtn == 0
	}

	return;
}

// Read stdout pipe, without blocking.
string
wml::Process::readAllStandardOutput (void)
{
	string s;
	int bytes = 0;
	char c;
	struct pollfd p;

	p.fd = this->childToParent[READING_END];
	p.events = POLLIN | POLLPRI;
	// We know we have at least one character to read, so seed revents
	p.revents = POLLIN;
	while (p.revents & POLLIN || p.revents & POLLPRI) {
		// This read of 1 byte should never block
		if ((bytes = read (this->childToParent[READING_END], &c, 1)) == 1) {
			s.append (1, c);
		}
		p.revents = 0;
		poll (&p, 1, 0);
	}
	return s;
}

// Read stderr pipe without blocking
string
wml::Process::readAllStandardError (void)
{
	string s;
	int bytes = 0;
	char c;
	struct pollfd p;

	p.fd = this->childErrToParent[READING_END];
	p.events = POLLIN | POLLPRI;
	// We know we have at least one character to read, so seed revents
	p.revents = POLLIN;
	while (p.revents & POLLIN || p.revents & POLLPRI) {
		// This read of 1 byte should never block because a poll() call tells us there is data
		if ((bytes = read (this->childErrToParent[READING_END], &c, 1)) == 1) {
			s.append (1, c);
		}
		p.revents = 0;
		poll (&p, 1, 0);
	}
	return s;
}

pid_t
wml::Process::getPid (void)
{
	return this->pid;
}

bool
wml::Process::running (void)
{
	if (this->pid > 0) {
		return true;
	}
	return false;
}

int
wml::Process::getError (void)
{
	return this->error;
}

void
wml::Process::setError (int e)
{
	this->error = e;
}

void
wml::Process::setCallbacks (ProcessCallbacks * cb)
{
	this->callbacks = cb;
}

//@}
