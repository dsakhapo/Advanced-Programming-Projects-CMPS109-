// $Id: scanner.cpp,v 1.8 2015-07-02 16:48:18-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#include <iostream>
#include <locale>
using namespace std;

#include "scanner.h"
#include "debug.h"

//scanner ctor()
scanner::scanner() {
   seen_eof = false;
   advance();
}

//For getting the next token from the input stream; tests for EOF
void scanner::advance() {
   if (not seen_eof) {
      cin.get(lookahead); //get one char at a time
      if (cin.eof()) seen_eof = true;
   }
}

token_t scanner::scan() {
   token_t result;
   while (not seen_eof and isspace(lookahead))
      advance();
   if (seen_eof) {
      result.symbol = tsymbol::SCANEOF;
   } else if (lookahead == '_' or isdigit(lookahead)) {
      result.symbol = tsymbol::NUMBER;
      do {//concatenate digits to form the correct number
         result.lexinfo += lookahead;
         advance();
      } while (not seen_eof and isdigit(lookahead));
   } else {
      result.symbol = tsymbol::OPERATOR;
      result.lexinfo += lookahead;
      advance();
   }
   DEBUGF('S', result);
   return result;
}

ostream& operator<<(ostream& out, const tsymbol& symbol) {
   switch (symbol) {
   case tsymbol::NUMBER:
      out << "NUMBER";
      break;
   case tsymbol::OPERATOR:
      out << "OPERATOR";
      break;
   case tsymbol::SCANEOF:
      out << "SCANEOF";
      break;
   }
   return out;
}

ostream& operator<<(ostream& out, const token_t& token) {
   out << token.symbol << ": \"" << token.lexinfo << "\"";
   return out;
}

