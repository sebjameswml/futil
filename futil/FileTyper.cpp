#ifdef __GNUG__
# pragma implementation
#endif

#include <WmlDbg.h>
#include <config.h>
#include <FileTyper.h>
#include <sstream>
#include <stdexcept>

using namespace std;

wml::FileTyper::FileTyper (void)
{
	//struct magic_set* magic;
	this->magic = magic_open (MAGIC_CHECK); // Can OR in MAGIC_MIME,
	                                        // MAGIC_RAW, etc for
	                                        // additional features. See
	                                        // file command's file.c for
	                                        // details.

	if (magic_load (this->magic, "/usr/share/wmlpp/wmlmagic.mgc") == -1) {
		// If the WML specific magic file is not found, fall
		// back to the one shipped with "file":
		if (magic_load (this->magic, "/usr/share/file/magic.mgc") == -1) {
			stringstream ee;
			ee << "Error loading file-typing info: " << magic_error (magic);
			throw runtime_error (ee.str());
		}
	}
}

wml::FileTyper::~FileTyper (void)
{
	if (this->magic != (struct magic_set*) 0) {
		magic_close (this->magic);
	}
}

string
wml::FileTyper::getFileTypeStr (string filePath)
{
	const char* type = magic_file (this->magic, filePath.c_str());
	if (type == (const char*)0) {
		stringstream ee;
		ee << "Error typing file: " << magic_error (this->magic);
		throw runtime_error (ee.str());
	}

	string fileType = type;

	return fileType;
}

wml::FILETYPE
wml::FileTyper::getFileType (string filePath)
{
	string fileTypeString = this->getFileTypeStr (filePath);
	if (fileTypeString.find ("Prescribe document") == 0) {
		return MFT_PRESCRIBE_DATA;
	} else if (fileTypeString.find ("PDF document") == 0) {
		return MFT_PDF_FILE;
	} else if (fileTypeString.find ("PostScript Type 1 font") != string::npos) {
		return MFT_TYPE1_FONT;
	} else if (fileTypeString.find ("TrueType") == 0) {
		DBG (fileTypeString);
		return MFT_TRUETYPE_FONT;
	} else if (fileTypeString.find ("PCL file") == 0) {
		return MFT_PCL_FILE;
	} else if (fileTypeString.find ("XML") == 0) {
		return MFT_XML;
	} else if (fileTypeString.find ("ASCII font metrics") == 0) {
		return MFT_AFM;
	} else if (fileTypeString.find ("ASCII") == 0) {
		return MFT_ASCII_TEXT;
	} else if (fileTypeString.find ("bzip2 compressed data") == 0) {
		return MFT_BZ2_ARCHIVE;
	} else if (fileTypeString.find ("gzip compressed data") == 0) {
		return MFT_GZIP_ARCHIVE;
	} else if (fileTypeString.find ("PPD file") == 0) {
		return MFT_PPD;
	} else if (fileTypeString.find ("RPM") == 0) {
		return MFT_RPM_PACKAGE;
	} else if (fileTypeString.find ("POSIX tar archive (GNU)") == 0) {
		return MFT_TAR;
	} else if (fileTypeString.find ("Debian binary package") == 0) {
		return MFT_DEBIAN_PACKAGE;
	} else if (fileTypeString.find ("Zip archive data") == 0) {
		return MFT_ZIP;
	} else if (fileTypeString.find ("PNG image") == 0) {
		return MFT_PNG_IMAGE;
	} else {
		return MFT_UNKNOWN;
	}
}
