/* -*-c++-*- */

#ifndef _FILETYPEIDENTIFIER_H_
#define _FILETYPEIDENTIFIER_H_

#ifdef __GNUG__
# pragma interface
#endif

#include <string>

extern "C" {
#include <magic.h>
}

namespace wml {

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
		std::string getFileType (std::string filePath);
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
