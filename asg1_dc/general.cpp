// $Id: general.cpp,v 1.5 2015-07-08 12:32:13-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#include <cstring>
using namespace std;

#include "general.h"

ydc_exn::ydc_exn(const string& what) :
      runtime_error(what) {
}

string exec::execname_; // Must be initialized from main().
int exec::status_ = EXIT_SUCCESS;

void exec::execname(const string& argv0) {
   execname_ = basename(argv0.c_str());
   cout << boolalpha;
   cerr << boolalpha;
   DEBUGF('Y', "execname = " << execname_);
}

void exec::status(int new_status) {
   new_status &= 0xFF;
   if (status_ < new_status)
      status_ = new_status;
}

ostream& note() {
   return cerr << exec::execname() << ": ";
}

ostream& error() {
   exec::status(EXIT_FAILURE);
   return note();
}

