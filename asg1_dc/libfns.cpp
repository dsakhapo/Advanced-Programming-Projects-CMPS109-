// $Id: libfns.cpp,v 1.4 2015-07-03 14:46:41-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#include "libfns.h"
#include "general.h"

//
// This algorithm would be more efficient with operators
// *=, /=2, and is_odd.  But we leave it here.
//
//largest value a long can be is 2,147,483,647
bigint pow (const bigint& base_arg, const bigint& exponent_arg) {
   bigint base (base_arg);
   bigint exponent (exponent_arg);
   static const bigint ZERO (0);
   static const bigint ONE (1);
   static const bigint TWO (2);
   static const bigint LONG_LIMIT ("2147483647");
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if(LONG_LIMIT < exponent) throw ydc_exn ("exponent too large");
   if (base == ZERO) return ZERO;
   bigint result = ONE;
   if (exponent < ZERO) {
      base = ONE / base;
      exponent = - exponent;
   }
   while (exponent > ZERO) {
      if (exponent % TWO == ONE) {
         result = result * base;
         exponent = exponent - 1;
      }else {
         base = base * base;
         exponent = exponent / 2;
      }
   }
   DEBUGF ('^', "result = " << result);
   return result;
}

