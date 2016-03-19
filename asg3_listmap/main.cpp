// $Id: main.cpp,v 1.8 2015-04-28 19:23:13-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
using namespace std;

#include "listmap.h"
#include "xpair.h"
#include "util.h"

using str_str_map = listmap<string,string>;
using str_str_pair = str_str_map::value_type;

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            traceflags::setflags (optarg);
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

//Removes trailing whitespace from the beginning and end of a string.
string trim(string line) {
   size_t start = 0;
   size_t end = 0;

   start = line.find_first_not_of(" ");
   end = line.find_last_not_of(" ");
   //This case is only for "="
   if (line == "") return "";
   line = line.substr(start, end - start + 1);
   return line;
}

// Prints out a line of output, consisting of the file name, the line
// number, and the string of output.
void print_line(const string file_name, int& line_num,
         string line_str) {
   cout << file_name << ": " << line_num << ": " << line_str << endl;
}

void format_line(string title, int line_num, string line,
         str_str_map &test) {
   string first; string second;
   size_t equal_sign_pos;
   str_str_map::iterator curr;

   //case: key
   if (line.find_first_of("=") == string::npos) {
      curr = test.find(line);
      if (curr == test.end()){
         print_line(title, line_num, line);
         cout << line << ": key not found" << endl;
      }
      else {
         print_line(title, line_num, line);
         cout << curr->first << " = " << curr->second << endl;
      }
     //All cases dealing with "="
   } else {
      //Find where "=" is.
      equal_sign_pos = line.find_first_of("=");

      if (equal_sign_pos == 0) {
         first = "";
         second = line.substr(equal_sign_pos + 1);
         second = trim(second);
         //case: =
         if (second == "") {
            print_line(title, line_num, "=");
            curr = test.begin();
            while (curr != test.end()) {
               cout << curr->first << " = " << curr->second << endl;
               ++curr;
            }
           //case: = value
         } else {
            print_line(title, line_num, line);
            curr = test.begin();
            while (curr != test.end()) {
               if (second == curr->second) {
                  cout << curr->first << " = " << curr->second << endl;
               }
               ++curr;
            }
         }
        //cases key =, key = value
      } else {
         first = line.substr(0, equal_sign_pos);
         second = line.substr(equal_sign_pos + 1);
         first = trim(first);
         second = trim(second);
         //case: key =
         if (second == "") {
            curr = test.find(first);
            if(curr == test.end()) print_line(title, line_num, line);
            else {
               curr = test.erase(curr);
               print_line(title, line_num, line);
            }
           //case: key = value
         } else {
            str_str_pair pair(first, second);
            curr = test.insert(pair);
            print_line(title, line_num, line);
            cout << curr->first + " = " + curr->second << endl;
         }
      }
   }
}

int main (int argc, char** argv) {
   sys_info::set_execname (argv[0]);
   scan_options (argc, argv);
   string line; int line_num = 1;
   str_str_map test;
   if(argc == optind){
      for(;;) {
         getline(cin, line);
         if(cin.eof()) break;
         //case: whitespace
         if (line == "") {
            print_line("-", line_num, line);
            ++line_num;
            continue;
         }
         else if (line.find_first_of("#")
          <= line.find_first_not_of(" ")) {
            print_line("-", line_num, line);
            ++line_num;
            continue;
         }
         format_line("-", line_num, line, test);
         ++line_num;
      }
   }
   else {
      for(char** argp = &argv[optind]; argp != &argv[argc]; ++argp){
          try{
            ifstream myfile(*argp);
            if(!myfile)
               throw processing_error("No such file or directory");
            while(getline(myfile, line)) {
               if (line == "") {
                  print_line(*argp, line_num, line);
                  ++line_num;
                  continue;
               } else if (line.find_first_of("#")
                        <= line.find_first_not_of(" ")) {
                  print_line(*argp, line_num, line);
                  ++line_num;
                  continue;
               }
               format_line(*argp, line_num, line, test);
               ++line_num;
            }
            myfile.close(); line_num = 1;
         }catch(processing_error& error){
            complain() << error.what() << endl;
         }
      }
   }
return sys_info::get_exit_status();
}

