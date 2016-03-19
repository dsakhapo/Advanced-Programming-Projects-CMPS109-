// $Id: bigint.h,v 1.28 2015-07-03 14:46:41-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#ifndef __BIGINT_H__
#define __BIGINT_H__

#include <exception>
#include <iostream>
#include <limits>
#include <utility>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

class bigint {
   friend ostream& operator<<(ostream&, const bigint&);
private:
   ubigint uvalue;
   bool is_negative { false };
public:

   bigint() = default; // Needed or will be suppressed.
   bigint(long);
   bigint(const ubigint&, bool is_negative = false);
   bigint(const bigint& that);
   explicit bigint(const string&);

   bigint operator+() const;
   bigint operator-() const;
   bigint operator+(const bigint&) const;
   bigint operator-(const bigint&) const;
   bigint operator*(const bigint&) const;
   bigint operator/(const bigint&) const;
   bigint operator%(const bigint&) const;

   bool operator==(const bigint&) const;
   bool operator<(const bigint&) const;
};

#endif

