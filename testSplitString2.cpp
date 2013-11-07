/*
 * A test of string-splitting functions. Concentrates on lots of tests to splitstring.
 */

#include "config.h"
#include "WmlDbg.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

std::ofstream DBGSTREAM;

#include "futil.h"

using namespace std;
using namespace wml;

int main (int argc, char **argv)
{
        DBGOPEN ("./testSplitString2.log");
        try {
                vector<string> samples;
                samples.push_back ("104520,FIRST INSURANCE SOLUTIONS LTD,\"Unit 6, The Oaks Business Village\",Revenge Road,Lordswood,Chatham,Kent,ME5 8LF,UK");
                samples.push_back ("104342,Towergate Risk Solutions (Lancaster),99 King Street,Lancaster,,,,LA1 1RH,UK");
                samples.push_back ("104788,CU Insurance Consultants Ltd,1 Trevor Terrace,North Shields,Tyne & Wear,,,NE30 2DG,UK");
                samples.push_back ("653,HOLMWOODS INSURANCE BROKERS LIMITED,LLOYDS,ONE LIME STREET,LONDON,,,EC3M 7HA,UK");
                samples.push_back ("658,SNEATH KENT STUART LIMITED,\"LLOYDS OF LONDON INSURANCE,\",\"LLOYDS BUILDING,1 LIME STREET,\",LONDON,,,EC3M 7HA,UK");
                samples.push_back ("663,E.J.WELTON,\"Lloyds Of London Insurance,\",\"Lloyds Building,1 Lime Street,\",London,,,EC3M 7HA,UK");
                samples.push_back ("664,THOMPSON HEATH & BOND LTD,\"Murray House,Murray Road\",,,Orpington,,BR5 3QY,UK");
                samples.push_back ("665,TOWERGATE STAFFORD KNIGHT & CO.LTD,55 Aldgate High Street,London,,,,EC3N 1AL,UK");
                samples.push_back ("666,HOLMANS CONNECT,London Underwriting Centre,3 Minster Court,Mincing Lane,London,,EC3R 7DD,UK");
                samples.push_back ("669,JOHN HOLMAN & SONS LTD,London Underwriting Centre,3 Minster Court,Mincing Lane,London,,EC3R 7DD,UK");
                samples.push_back ("670,ROGER LARK & SEDGWICK,\"LLOYDS OF LONDON INSURANCE,\",\"LLOYDS BUILDING,1 LIME STREET,\",LONDON,,,EC3M 7HA,UK");
                samples.push_back ("104557,Willis & Company (Insurance Brokers) Ltd,55-59 Donegall Street,Belfast,Co Antrim,,,BT1 2FH,UK");
                samples.push_back ("104585,Lionheart Insurance Services Ltd,Radio House,Aston Road North,Birmingham,,,B6 4DA,UK");
                samples.push_back ("105097,Oamps (UK) Ltd (Wholesale),Windsor House,High Street,Esher,,,KT10 9RY,UK");
                samples.push_back ("104837,Lansdowne Woodward Ltd,Princes Court,Princes Road,Ferndown,,,BH22 9JG,UK");
                samples.push_back ("104676,P J Mcilroy & Son Insurance & Investment,13 Ann Street,Ballycastle,Co. Antrim,,,BT54 6AA,UK");
                samples.push_back (",,,,,,,,");
                samples.push_back ("104980,D N Paterson & Company,17 Sandhead Road,Strathaven,Lanarkshire,,,ML10 6HX,UK");
                samples.push_back ("104692,Sterling General Insurance Services Ltd,81 Albany Street,Regents Park,,,,NW1 4BT,UK");


                string seps(",");
                string encs("\"");
                unsigned int i_count = 0;
                vector<string> toks3;
                vector<string>::iterator t; // token iterator
                vector<string>::const_iterator s = samples.begin(); // sample iterator.


                while (s != samples.end()) {

                        toks3.clear();
                        toks3 = futil::splitStringWithEncs (*s, seps, encs);
                        cout << "Output of splitStringWithEncs for the string:" << endl;
                        // A count line:
                        cout << "!123456789\"123456789£123456789$123456789\%123456789^123456789&123456789*123456789(123456789)123456789" << endl << *s << endl;
                        i_count = 0;
                        for (t = toks3.begin(); t != toks3.end(); t++) {
                                cout << (i_count++) << ". " << *t << "\n";
                        }

                        ++s;
                }

        } catch (const exception& e) {
                // handle error condition; should catch any
                // exception thrown above
                DBGSTREAM << "Exception: '" << e.what() << "'\n";
                cerr << "Exception: '" << e.what() << "'\n";
                DBGSTREAM.flush();
        }

        DBGCLOSE();
        _exit(0);
}
