// $Id: bigint.cpp,v 1.73 2015-07-03 14:46:41-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint(const bigint& that) {
   is_negative = that.is_negative;
   uvalue = that.uvalue;
}
bigint::bigint(long that) :
      uvalue(that), is_negative(that < 0) {
   DEBUGF('~', this << " -> " << uvalue)
}

bigint::bigint(const ubigint& uvalue, bool is_negative) :
      uvalue(uvalue), is_negative(is_negative) {
}

bigint::bigint(const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint(that.substr(is_negative ? 1 : 0));
}

bigint bigint::operator+() const {
   return *this;
}

bigint bigint::operator-() const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+(const bigint& that) const {
   bool result_sign = false; //true if negative, false if positive
   ubigint result;
   //If both operands are the same sign,
   //will check sign, and assign accordingly
   if (is_negative == that.is_negative) {
      if (is_negative == true)
         result_sign = true;
      result = uvalue + that.uvalue;
   }
   //If operands are diff signs, and right op is greater
   ////treat the equation as a subtraction, right - left.
   else if (uvalue < that.uvalue) {
      if (that.is_negative)
         result_sign = true;
      result = that.uvalue - uvalue;
   }
   //If operands are diff signs, and left op is greater
   //sign will reflect what sign left op is
   else {
      if (is_negative)
         result_sign = true;
      result = uvalue - that.uvalue;
   }
   return {result, result_sign};
}

bigint bigint::operator-(const bigint& that) const {
   bool result_sign = false;
   ubigint result;
   //If operands have diff signs check if the
   //left op is neg, treat as addition,
   //and make result neg
   if (is_negative != that.is_negative) {
      if (is_negative)
         result_sign = true;
      result = uvalue + that.uvalue;
   }
   //If the right operand is greater,
   //and they are the same sign,
   //check to see if left op is pos,
   //treat the operation as a subtraction
   else if (uvalue < that.uvalue) {
      if (is_negative == false)
         result_sign = true;
      result = that.uvalue - uvalue;
   }
   //If the values are the same, set 0 to pos
   else {
      if (uvalue == that.uvalue)
         ;
      else if (is_negative)
         result_sign = true;
      result = uvalue - that.uvalue;
   }
   return {result, result_sign};
}

bigint bigint::operator*(const bigint& that) const {
   bool result_sign = false;
   ubigint result;
   if (is_negative and that.is_negative)
      ; //If both are negative, do nothing.
   else if (is_negative or that.is_negative)
      result_sign = true; //If only one of the operands are negative,
   result = uvalue * that.uvalue; //make the result negative.
   return {result, result_sign};
}

bigint bigint::operator/(const bigint& that) const {
   bool result_sign = false;
   ubigint result;
   if (is_negative and that.is_negative)
      ; // If both are negative, do nothing.
   else if (is_negative or that.is_negative)
      result_sign = true; //If only one of the operands are negative
   result = uvalue / that.uvalue; //make the result negative.
   return {result, result_sign};
}

bigint bigint::operator%(const bigint& that) const {
   bool result_sign = false;
   ubigint result;
   if (is_negative and that.is_negative)
      ;
   else if (is_negative or that.is_negative)
      result_sign = true;
   result = uvalue % that.uvalue;
   return {result, result_sign};
}

bool bigint::operator==(const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator<(const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? that.uvalue < uvalue:uvalue < that.uvalue;
}

ostream& operator<<(ostream& out, const bigint& that) {
   return out << (that.is_negative ? "-" : "")
         << that.uvalue;
}

