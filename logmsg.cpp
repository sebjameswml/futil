/*!
 *
 *  A trivial little program to make a syslog message from the command
 *  line.
 *
 *  This program is part of WML futil - a library containing static
 *  public utility functions and classes used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
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

#include <string>
#include <iostream>
#include <syslog.h>

#include "futil/config.h"

using namespace std;

int main(int argc, char** argv)
{
        if (argc<2) {
                cerr << "Usage: " << argv[0] << " Message to syslog" << endl;
                cerr << "Messages are syslogged with LOG_INFO level." << endl;
                return -1;
        }

        openlog ("logmsg", LOG_PID, LOG_USER);

        string msg("");
        int i = 1;
        bool first = true;

        while (i < argc) {
                if (first == true) {
                        // don't add preceding space.
                        first = false;
                } else {
                        msg += " ";
                }
                msg += argv[i];
                ++i;
        }

        syslog (LOG_INFO, "%s", msg.c_str());
        closelog();
        return 0;
}
