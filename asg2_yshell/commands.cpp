// $Id: commands.cpp,v 1.16 2016-01-14 16:10:40-08 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)

#include "commands.h"
#include "debug.h"

command_hash cmd_hash {
   {"#"     , fn_comm  },
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int exit_status = exit_status::get();
   cout << execname() << ": exit(" << exit_status << ")" << endl;
   return exit_status;
}

// Comment function. Doesn't return or do anything.
void fn_comm(inode_state& state, const wordvec& words) {
   // This has been intentionally left empty;
   DEBUGF('c', state);
   DEBUGF('c', words);
}

void fn_cat(inode_state& state, const wordvec& words) {
   if(words.size() == 1)
      throw command_error("fn_cat: no args specified");
   if (words.size() > 1) {
      state.read_file(state.get_cwd(), words);
   }
      DEBUGF('c', state);
      DEBUGF('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   state.change_directory(state, words);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}

// Exit function. If exit is called with arguments, the arguments are
// parsed as exit status. If the argument is an int, that int will be
// returned. If it is not an int, the int 127 will be passed instead.
void fn_exit (inode_state& state, const wordvec& words){
   if (words.size() > 1) {
      exit_status e;
      string s = "";
      bool alpha = false;
      for (size_t i = 1; i < words.size(); ++i)
         s += words.at(i);

      for (size_t j = 0; j != s.size(); ++j) {
         if (isalpha(s[j]) == true) {
            alpha = true;
         }
      }
      if (alpha == true) {
         e.set(127);
      } else {
         e.set(stoi(s));
      }
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

// Displays the entities within a current directory, including files
// and other directories.
void fn_ls (inode_state& state, const wordvec& words){
   if(words.size() <= 2){
      state.print_directory(state.get_cwd(), words);
   }
   else throw command_error("fn_ls: invalid num of args");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   if(words.size() <= 2){
      state.list_recursively(state, words);
   }
   else throw command_error("fn_lsr: invalid num of args");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   state.create_file(state.get_cwd(), words);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   if(words.size() == 1) throw command_error("fn_mkdir: no arg");
   else if(words.size() == 2){
      state.make_directory(state.get_cwd(), words);
   }
   else throw command_error("fn_mkdir: invalid arg");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

// Changes the character to be used as the prompt character.
void fn_prompt (inode_state& state, const wordvec& words){
   string new_prompt = "";
   for (size_t i = 1; i < words.size(); ++i) {
      new_prompt += words.at(i);
      new_prompt += " ";
   }
   state.set_prompt(new_prompt);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   if(words.size() == 1) state.print_path(state.get_cwd());
   else throw command_error("fn_pwd: invalid num of args");
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   state.remove(state.get_cwd(), words);
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}
//NEED TO DO
void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

