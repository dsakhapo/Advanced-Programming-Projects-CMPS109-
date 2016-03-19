// $Id: cix.cpp,v 1.2 2015-05-12 18:59:40-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream log (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", CIX_EXIT},
   {"help", CIX_HELP},
   {"ls"  , CIX_LS  },
   {"get" , CIX_GET },
   {"put" , CIX_PUT },
   {"rm"  , CIX_RM  },
};

void cix_help() {
   static vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = CIX_LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != CIX_LSOUT) {
      log << "sent CIX_LS, server did not return CIX_LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer;
   }
}

void cix_get(client_socket& server, string filename) {
   // Filename format checking.
   if (filename.length() >= FILENAME_SIZE) {
      log << filename << ": that filename is too long." << endl;
      return;
   } else if (filename.find('/') != string::npos) {
      log << filename << ": filenames cannot have the / character."
               << endl;
      return;
   }
   cix_header header;
   header.command = CIX_GET;
   strcpy(header.filename, filename.c_str());
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != CIX_FILE) {
      log << "sent CIX_GET, server did not return CIX_FILE" << endl;
      log << "server returned " << header << endl;
      log << filename << " was not saved." << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      // Creates a new file to match the server one.
      ofstream outfile (filename);   // Rename to file name soon.
      // The server file's contents are copied into the new file.
      outfile << buffer << endl;
      outfile.close();
      log << filename << " saved successfully." << endl;
   }
}

void cix_put (client_socket& server, string filename) {
   if (filename.length() >= FILENAME_SIZE) {
      log << filename << ": that filename is too long." << endl;
      return;
   } else if (filename.find('/') != string::npos) {
      log << filename << ": filenames cannot have the / character."
               << endl;
      return;
   }
   cix_header header;
   strcpy(header.filename, filename.c_str());
   FILE* put_pipe = fopen(header.filename, "r");
   char buffer[0x1000];
   if (put_pipe == NULL) {
         log << "put: fopen failed: " << strerror (errno) << endl;
      }
   else{
      string file_contents;
      while (!feof(put_pipe)) {
         char* rc = fgets(buffer, sizeof buffer, put_pipe);
         if (rc == nullptr) break;
         file_contents.append(buffer);
      }
      int status = pclose(put_pipe);
      if (status < 0)
         log << header.filename << ": " << strerror(errno) << endl;
      else
         log << header.filename << ": exit " << (status >> 8)
                  << " signal " << (status & 0x7F) << " core "
                  << (status >> 7 & 1) << endl;
      header.command = CIX_PUT;
      header.nbytes = file_contents.size();
      log << "sending header " << header << endl;
      send_packet(server, &header, sizeof header);
      send_packet(server, file_contents.c_str(), file_contents.size());
      recv_packet(server, &header, sizeof header);
      log << "sent " << file_contents.size() << " bytes" << endl;
      if (header.command != CIX_ACK) {
         log << "Error, sent CIX_PUT and received: " << header << endl;
      }
   }
}

void cix_rm (client_socket& server, string filename) {
   // Filename format checking.
   if (filename.length() >= FILENAME_SIZE) {
      log << filename << ": that filename is too long." << endl;
      return;
   } else if (filename.find('/') != string::npos) {
      log << filename << ": filenames cannot have the / character."
               << endl;
      return;
   }
   cix_header header;
   header.command = CIX_RM;
   strcpy(header.filename, filename.c_str());
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != CIX_ACK) {
      log << "sent CIX_RM, server did not return CIX_ACK" << endl;
      log << "server returned " << header << endl;
      log << filename << " cannot be removed." << endl;
   }else {
      log << filename << " removed successfully." << endl;
   }
}

void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);

   // Fix for when there is only one argument.
   if(args.size() == 1) {
      size_t pos = args[0].find(".");
      if(pos == string::npos){
         // If no . in the arg, the arg must be a port number.
         host = get_cix_server_host (args, 1);
         port = get_cix_server_port (args, 0);
      } else {
         // If there is a ., the arg must be a host IP address.
         host = get_cix_server_host (args, 0);
         port = get_cix_server_port (args, 1);
      }
   }

   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         // Splits the argument line
         string command_name{}, file_name{};
         size_t split_pos = line.find(" ");
         if (split_pos == string::npos) {
            command_name = line;
         } else {
            command_name = line.substr(0, split_pos);
            file_name = line.substr(split_pos + 1);
//            cout << command_name << endl << file_name << endl;
         }
         if (cin.eof()) throw cix_exit();
         log << "command " << line << endl;
         const auto& itor = command_map.find (command_name);
         cix_command cmd = itor == command_map.end()
                         ? CIX_ERROR : itor->second;
         switch (cmd) {
            case CIX_EXIT:
               throw cix_exit();
               break;
            case CIX_HELP:
               cix_help();
               break;
            case CIX_LS:
               cix_ls (server);
               break;
            case CIX_GET:
               cix_get (server, file_name);
               break;
            case CIX_PUT:
               cix_put (server, file_name);
               break;
            case CIX_RM:
               cix_rm (server, file_name);
               break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

