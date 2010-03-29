#ifdef __GNUG__
# pragma implementation
#endif

#include <config.h>
#include <FileTypeIdentifier.h>
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
wml::FileTyper::getFileType (string filePath)
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
