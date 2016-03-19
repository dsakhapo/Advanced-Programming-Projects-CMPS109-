// $Id: debug.cpp,v 1.6 2015-07-02 16:48:18-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#include <climits>
#include <iostream>
#include <vector>
using namespace std;

#include "debug.h"
#include "general.h"

vector<bool> debugflags::flags(UCHAR_MAX + 1, false);

void debugflags::setflags(const string& initflags) {
   for (const unsigned char flag : initflags) {
      if (flag == '@')
         flags.assign(flags.size(), true);
      else
         flags[flag] = true;
   }
   // Note that DEBUGF can trace setflags.
   if (getflag('x')) {
      string flag_chars;
      for (size_t index = 0; index < flags.size(); ++index) {
         if (getflag(index)) flag_chars += static_cast<char>(index);
      }
      DEBUGF('x', "debugflags::flags = " << flag_chars);
   }
}

//
// getflag -
//    Check to see if a certain flag is on.
//

bool debugflags::getflag(char flag) {
   return flags[static_cast<unsigned char>(flag)];
}

void debugflags::where(char flag, const char* file, int line,
         const char* func) {
   note() << "DEBUG(" << flag << ") " << file << "[" << line << "] "
            << func << "()" << endl;
}

