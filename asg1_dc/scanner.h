// $Id: scanner.h,v 1.3 2015-07-02 16:48:18-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <iostream>
#include <utility>
using namespace std;

#include "debug.h"

enum class tsymbol {
   SCANEOF, NUMBER, OPERATOR
};
struct token_t {
   tsymbol symbol;//Will be one of the following:SCANEOF,NUMBER,OPERATOR
   string lexinfo;
};

class scanner {
private:
   bool seen_eof;
   char lookahead;
   void advance();
public:
   scanner();
   token_t scan();
};

ostream& operator<<(ostream&, const tsymbol&);
ostream& operator<<(ostream&, const token_t&);

#endif

