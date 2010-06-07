/* -*-c++-*- */

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
	 * File page enumerated types
	 */
	//@{

        /*!
         * \brief The types of files which a user of the Tempest PDF+
         * or DSM might upload
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
		FILETYPE_N
	};

	//@}

	class FileTyper
	{
	public:
		/*! Constructor and destructor */
		FileTyper (void);
		~FileTyper (void);

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
