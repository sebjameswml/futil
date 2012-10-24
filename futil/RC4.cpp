#ifdef __GNUG__
#  pragma implementation
#endif

#include "config.h"
#include "WmlDbg.h"
#include "RC4.h"

#include <vector>

using namespace std;
using namespace wml;

string
wml::RC4::rc4crypt (const string& key, const string& input)
{
        DBG2 ("Key is: " << key);
        DBG2 ("Input is: " << input);

        string rtn("");

        string::size_type keylength (key.size());
        unsigned int i(0), j(0);
        unsigned char tmp('\0');

        // RC4 key-scheduling algorithm (KSA)
        vector<unsigned char> perm;
        for (i = 0; i < 256; i++) {
                perm.push_back (i);
        }
        for (i = 0; i < 256; i++) {
                j = (j + perm.at(i) + key.at (i % keylength)) % 256;
                // Swap values of perm[i] and perm[j].
                tmp = perm.at(i);
                perm.at(i) = perm.at(j);
                perm.at(j) = tmp;
        }

        // Pseudo-random generation algorithm (PRGA)
        i = 0;
        j = 0;
        string::const_iterator iter (input.begin()), end (input.end());
        while (iter != end) {

                i = (i + 1) % 256;
                j = (j + perm.at(i)) % 256;

                // Swap values of perm[i] and perm[j].
                tmp = perm.at(i);
                perm.at(i) = perm.at(j);
                perm.at(j) = tmp;

                // Generate keystream value.
                tmp = perm.at ((perm.at(i) + perm.at(j)) % 256);

                // XOR the keystream value with the next byte of the
                // message to produce the next byte of either
                // ciphertext or plaintext.
                rtn += *iter ^ tmp;

                ++iter;
        }
        DBG2 ("Output is: " << rtn);
        return rtn;
}
