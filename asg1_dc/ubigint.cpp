// $Id: ubigint.cpp,v 1.8 2015-07-03 14:46:41-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include<string>
using namespace std;

#include "ubigint.h"
#include "debug.h"

ubigint::ubigint(const int& value) {
   ubig_value.push_back(value + '0');
   // DEBUGF ('~', this << " -> " << ubig_value)
}

//ctor(), this will construct a ubigint object that inserts
//the string char by char into a char vector ubig_value, backwards
ubigint::ubigint(const string& that) {
   string::const_iterator i = that.end() - 1;
   while (i >= that.begin()) {
      ubig_value.push_back(*i);
      --i;
   }
}

//this vector sum algorithm will first:
//step 1: convert the first digit of each vector to an int & add them
//step 2: if cf, add 1 to sum and set cf to false, otherwise do nothing
//step 3: if sum>9, convert to string, otherwise, result = sum
//step 4: if sum>9 it's now a string, result = low order digit
//and set cf (carry flag)
ubigint ubigint::operator+(const ubigint& that) const {
   int left, right, sum; //left, right operand digits and their sum
   ubigint result;
   vector<udigit_t>::const_iterator l, r;
   l = ubig_value.begin();
   r = that.ubig_value.begin();
   bool cf = false; //carry flag will emulate carry operations.
   while (l != ubig_value.end() and r != that.ubig_value.end()) {
      left = *l - '0';
      right = *r - '0';
      sum = left + right;
      if (cf) { //If the carry flag is set from the previous loop,
         ++sum; //emulate carry of 10
         cf = false;
      }
      //If the sum is greater than 9, then it must emulate a carry.
      if (sum > 9) {
         string s = to_string(sum);
         result.ubig_value.push_back(s[1]);
         cf = true;
      } else
         result.ubig_value.push_back(sum + '0');
      ++l;
      ++r;
   }
   //One of these paths is taken, depending on which operand ran out
   //of digits
   if (ubig_value.size() > that.ubig_value.size()) {
      while (l != ubig_value.end()) {
         left = *l - '0';
         right = 0;
         sum = left + right;
         if (cf) { //If the carry flag is set from the previous loop,
            ++sum;
            cf = false;
         }
         //If the sum is greater than 9, then it must emulate a carry.
         if (sum > 9) {
            string s = to_string(sum);
            result.ubig_value.push_back(s[1]);
            cf = true;
         } else
            result.ubig_value.push_back(sum + '0');
         ++l;
      }
   } else if (ubig_value.size() < that.ubig_value.size()) {
      while (r != that.ubig_value.end()) {
         left = 0;
         right = *r - '0';
         sum = left + right;
         if (cf) { //The same carry flag operations occur here as well.
            ++sum;
            cf = false;
         }
         if (sum > 9) {
            string s = to_string(sum);
            result.ubig_value.push_back(s[1]);
            cf = true;
         } else
            result.ubig_value.push_back(sum + '0');
         ++r;
      }
   }
   //If the end of the result value is reached,
   //but the cf is still true, then we emulate a carry
   //in the highest magnitude of the result number.
   if (cf) {
      result.ubig_value.push_back('1');
   }

   //Get rid of leading 0's
   l = result.ubig_value.end() - 1;
   while (*l == '0') {
      result.ubig_value.pop_back();
      --l;
   }
   return result;
}
//this vector subtraction algorithm will first
//step 1: convert the first digit of each vector into an int & subtract
//step 2: if the left operand < right, 10 will be
//added to the left and carry flag will be set
//step 3: each difference is pushed into the result
//step 4: each iteration will check the cf flag,
         //if it's set, it will subtract 1 from the left operand
ubigint ubigint::operator-(const ubigint& that) const {
   if (*this < that) throw domain_error("ubigint::operator-(a<b)");
   int left, right, diff; //diff for difference
   ubigint result;
   vector<udigit_t>::const_iterator l, r;
   l = ubig_value.begin();
   r = that.ubig_value.begin();
   bool cf = false; //carry flag, if set will subtract 1 from left op
   bool zf = false; //zero flag, if set, means that a zero has borrowed.
   while (l != ubig_value.end() and r != that.ubig_value.end()) {
      left = *l - '0';
      right = *r - '0';
      //used to determine the first non-zero
      //occurrence after contiguous 0's
      if (zf and left != 0) {
         --left;
         zf = false;
      }
      //if cf was set in the previous loop,
      //removes one from the current value to emulate a carry.
      if (cf) {
         --left;
         cf = false;
      }
      if (left < right) {
         //This will check to see if a zero needs to borrow,
         //and we set the zf to indicate this
         if (zf == false and left == 0) {
            left = 10;
            zf = true;
         }
         //If 0's after a previous zero borrowed,
         //we simulate a borrow from this 0
         else if (zf and left == 0) {
            left = 9;
         } else {
            left = left + 10; // Emulates a carry operation.
            cf = true;
         }
      }
      diff = left - right;
      result.ubig_value.push_back(diff + '0');
      ++l, ++r;
   }

   //If the right operand runs out of digits,
   //check to see if cf and zf flags were set first,
   //then proceed to finish adding digits to result
   while (l != ubig_value.end()) {
      left = *l - '0';
      //If there was a borrow from a zero,
      //this will provide the carry operation
      if (zf and left != 0) {
         --left;
         zf = false;
         result.ubig_value.push_back(left + '0');
         ++l;
      }
      //If there was a borrow from a zero,
      //and the current value is still zero,
      else if (zf and left == 0) {
         left = 9; //set the value to 9 to emulate a carry
         result.ubig_value.push_back(left + '0');
         ++l;
      }
      //If a non-zero carry operation occurred,
      //just decrease the number by one
      else if (cf) {
         left = *l - '0'; //to emulate a carry.
         --left;
         result.ubig_value.push_back(left + '0');
         cf = false;
         ++l;
      } else {
         result.ubig_value.push_back(*l);
         ++l;
      }
   }

   //pop all high order 0's
   l = result.ubig_value.end() - 1;
   if (*l == '0') {
      while (*l == '0' and l != result.ubig_value.begin()) {
         result.ubig_value.pop_back();
         --l;
      }
   }
   return result;
}

//Multiplication algorithm provided
//Please review the instructions for details of the algorithm.
ubigint ubigint::operator*(const ubigint& that) const {
   int carry, product;
   ubigint result;
   result.ubig_value.resize(ubig_value.size() + that.ubig_value.size(),
            '0');
   for (int m = 0; m < ubig_value.size(); ++m) {
      carry = 0;
      for (int n = 0; n < that.ubig_value.size(); ++n) {
         product = (result.ubig_value.at(m + n) - '0')
                  + ((ubig_value.at(m) - '0')
                           * (that.ubig_value.at(n) - '0') + carry);
         result.ubig_value.at(m + n) = product % 10 + '0';
         carry = product / 10;
      }
      result.ubig_value.at(m + that.ubig_value.size()) = carry + '0';
   }
   while (result.ubig_value.size() > 1
            and result.ubig_value.back() == '0') {
      result.ubig_value.pop_back();
   }
   return result;
}

void ubigint::multiply_by_2() {
   int digit;
   vector<udigit_t>::iterator i;
   vector<udigit_t> result;
   i = ubig_value.begin();
   bool cf = false; //Carry flag.
   while (i != ubig_value.end()) {
      digit = (*i - '0') * 2;
      if (cf) {
         ++digit;
         cf = false;
      }
      if (digit > 9) {
         string s = to_string(digit);
         result.push_back(s[1]);
         cf = true;
      } else {
         result.push_back(digit + '0');
      }
      ++i;
   }
   if (cf) result.push_back('1');
   ubig_value.clear();
   i = result.begin();
   while (i != result.end()) {
      ubig_value.push_back(*i);
      ++i;
   }
}

void ubigint::divide_by_2() {
   int div_result, remainder; //Division Result, and Remainder
   div_result = remainder = 0;
   vector<udigit_t> result;
   string s = "";
   vector<udigit_t>::iterator i; //i for indexing into the vector
   i = ubig_value.end() - 1;
   //If there is a remainder, take the current value and append,
   //this code will take the emulated large value, turn it from a string
   //to an int, and do operations upon it
   while (i >= ubig_value.begin()) {
      if (remainder != 0) {
         s = to_string(*i - '0');
         s = to_string(remainder) + s;
         div_result = stoi(s) / 2;
         remainder = stoi(s) % 2;
         result.push_back(div_result + '0');
         --i;
      }
      //If the first digit isn't div by 2
      //If the value is zero, force a zero to be added since 0/2=0.
      else if ((*i - '0') / 2 <= 0) {
         if ((*i - '0') == 0) {
            result.push_back('0');
            --i;
            //first two lines will combine chars to form a two digit num
         } else {
            s = to_string(*i - '0');
            --i;
            s = s + to_string(*i - '0');
            --i;
            div_result = stoi(s) / 2;
            remainder = stoi(s) % 2;
            result.push_back('0');
            result.push_back(div_result + '0');
         }
      } else {
         div_result = (*i - '0') / 2;
         remainder = (*i - '0') % 2;
         result.push_back(div_result + '0');
         --i;
      }
   }
   //insert the result in the correct order into ubig_value
   ubig_value.clear();
   i = result.end() - 1;
   while (i >= result.begin()) {
      ubig_value.push_back(*i);
      --i;
   }
   //pop all high-order 0's
   i = ubig_value.end() - 1;
   while (*i == '0') {
      ubig_value.pop_back();
      --i;
   }
}

ubigint::quot_rem ubigint::divide(const ubigint& that) const {
   static const ubigint zero = 0;
   if (that == zero) throw domain_error("ubigint::divide: by 0");
   ubigint power_of_2 = 1;
   ubigint divisor = that; //right operand, divisor
   ubigint quotient = 0;
   ubigint remainder = *this; //left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return
   {  quotient, remainder};
}

ubigint ubigint::operator/(const ubigint& that) const {
   return divide(that).first;
}

ubigint ubigint::operator%(const ubigint& that) const {
   return divide(that).second;
}

bool ubigint::operator==(const ubigint& that) const {
   int left, right;
   vector<udigit_t>::const_iterator l, r;
   l = ubig_value.end() - 1;
   r = that.ubig_value.end() - 1;
   //If the two values are not the same size...
   if (ubig_value.size() != that.ubig_value.size()) {
      //iterate through them to find differences.
      while (l >= ubig_value.begin() and r >= that.ubig_value.begin()) {
         left = *l - '0';
         right = *r - '0';
         //If you iterate through the values and find mismatched values,
         if (left != right) {
            return false; //they are thus not equivalent.
         } else {
            --l;
            --r;
         }
      }
      return true;
   }
   //If the numbers being compared are the same size
   while (l >= ubig_value.begin() and r >= that.ubig_value.begin()) {
      left = *l - '0';
      right = *r - '0';
      //If you iterate through the values and find mismatched values,
      if (left != right) {
         return false; //they are thus not equivalent.
      } else {
         --l;
         --r;
      }
   }
   return true;
}

bool ubigint::operator<(const ubigint& that) const {
   //If the magnitude of Left is smaller than Right, then Left < Right.
   if (ubig_value.size() < that.ubig_value.size())
      return true;
   //Contra-positive of the above statement.
   else if (ubig_value.size() > that.ubig_value.size())
      return false;
   else { //If the two sides are of equal magnitude...
      int left, right;
      vector<udigit_t>::const_iterator l, r;
      l = ubig_value.end() - 1;
      r = that.ubig_value.end() - 1;

      while (l >= ubig_value.begin() and r >= that.ubig_value.begin()) {
         left = *l;
         right = *r;
         if (left < right) {
            return true;
         } else if (left > right) {
            return false;
         } else {
            --l;
            --r;
         }
      }
   }
   return false;
}

ostream& operator<<(ostream& out, const ubigint& that) {
   vector<unsigned char>::const_iterator i = that.ubig_value.end() - 1;

   //This code allows us to segment our code into lines of
   //69 characters and a / as the 70th character.
   int counter = 0;
   while (i >= that.ubig_value.begin()) {
      if (counter == 69) { //dc uses 69 characters, and so will we!
         out << "\\" << endl;
         counter = 0;
      }
      out << *i;
      --i;
      ++counter;
   }
   return out;
}

