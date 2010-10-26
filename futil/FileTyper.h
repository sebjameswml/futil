/* -*-c++-*- */
/*
 * \file FileTyper.h
 *
 * \brief A c++ wrapper around libmagic
 *
 *  This file is part of WML futil - a library containing static public
 *  utility functions and classes used across WML code.
 *
 *  WML futil is Copyright William Matthew Ltd. 2010.
 *
 *  Authors: Seb James <sjames@wmltd.co.uk>
 *           Tamora James <tjames@wmltd.co.uk>
 *           Mark Richardson <mrichardson@wmltd.co.uk>
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

#ifndef _FILETYPER_H_
#define _FILETYPER_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <string>

extern "C" {
#include <magic.h>
}

namespace wml {

        /*!
         * \brief The types of files which are used in WMLPP
         * appliances.
	 *
	 * Why the MFT prefix? MFT stands for
         * "Macro File Type". When I first had to identify files it
         * was to determine what kind of PCL macro they were. The MFT
         * prefix has simply got stuck.
         */
        enum FILETYPE {
		MFT_UNKNOWN,
		MFT_FORM,
		MFT_MACRO,
		MFT_TYPE1_FONT,
		MFT_TRUETYPE_FONT,
		MFT_CONFIG,
		MFT_UNICODE_FONT,
		MFT_UNICODE_ARCHIVE,
		MFT_ZIP,
		MFT_PNG_IMAGE,
		MFT_PDF_FILE,
		MFT_CONVERT_INI,
		MFT_USER_INI,
		MFT_CSV,
		MFT_XML,
		MFT_LOG,
		MFT_RAW_DATA,
		MFT_AFM,
		MFT_PPD,
		MFT_PCL_FILE,
		MFT_PRESCRIBE_DATA,
		MFT_ASCII_TEXT,
		MFT_ISO8859_TEXT,
		MFT_BZ2_ARCHIVE,
		MFT_GZIP_ARCHIVE,
		MFT_RPM_PACKAGE,
		MFT_DEBIAN_PACKAGE,
		MFT_TAR,
		MFT_POSTSCRIPT,
		MFT_HP_PJL,
		FILETYPE_N
	};

	/*!
	 * \brief A wrapper around libmagic
	 *
	 * Makes use of libmagic to identify files wrapping that api
	 * up in a simple C++ interface.
	 */
	class FileTyper
	{
	public:
		/*! Constructor and destructor */
		//@{
		FileTyper (void);
		~FileTyper (void);
		//@}

		/*!
		 * \brief Return a string containing information
		 * on the type of file
		 */
		std::string getFileTypeStr (std::string filePath);

		/*!
		 * \brief Return a wml::FILETYPE based on what
		 * type of file is passed in
		 */
		wml::FILETYPE getFileType (std::string filePath);
	private:
		/*!
		 * Pointer to libmagic struct containing magic file type
		 * rules
		 * This is initialised in the constructor which allocates
		 * memory and then uninitialised in the deconstructor
		 */
		struct magic_set* magic;
	};
}

#endif
