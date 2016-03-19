// $Id: interp.cpp,v 1.2 2015-07-16 16:57:30-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"moveby" , &interpreter::do_moveby },
   {"border" , &interpreter::do_border },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"        , &interpreter::make_text        },
   {"ellipse"     , &interpreter::make_ellipse     },
   {"circle"      , &interpreter::make_circle      },
   {"polygon"     , &interpreter::make_polygon     },
   {"rectangle"   , &interpreter::make_rectangle   },
   {"square"      , &interpreter::make_square      },
   {"diamond"     , &interpreter::make_diamond     },
   {"triangle"    , &interpreter::make_triangle    },
   {"equilateral" , &interpreter::make_equilateral },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}

void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}

void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}

void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   //itor->second->draw (where, color);
   window::push_back(object(where, color, itor->second));
}

void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   window::moveby(stoi(*begin));
}

void interpreter::do_border(param begin, param end){
   DEBUGF ('f', range (begin, end));
   window::border(begin[0], stoi(begin[1]));
}

shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   void* font = search_fontcode(*begin);
   string textbody;
   bool space = false;
   for (auto itor = ++begin; itor != end; ++itor){
      if (space) textbody += " ";
      textbody += *itor;
      space = true;
   }
   return make_shared<text> (font, textbody);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 2) throw runtime_error
            ("Make_ellipse: invalid number of args. Need 2.");
   return make_shared<ellipse> (GLfloat(stof(begin[0])),
                                GLfloat(stof(begin[1])));
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 1) throw runtime_error
   ("make_circle: invalid number of args. Need 1.");
   return make_shared<circle> (GLfloat(stof(begin[0])));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(((end - begin) % 2 )== 1 ) throw runtime_error
         ("make_polygon: invalid number of args. Need an even number.");
   vertex_list vert_list{};
   for (auto itor = begin; itor != end; ++itor) {
      vertex vert{stof(*itor++), stof(*itor)};
      vert_list.push_back(vert);
   }
   return make_shared<polygon> (vert_list);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 2) throw runtime_error
            ("make_rectangle: invalid number of args. Need 2.");
   return make_shared<rectangle> (GLfloat(stof(begin[0])),
            GLfloat(stof(begin[1])));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 1) throw runtime_error
            ("make_square: invalid number of args. Need 1.");
   return make_shared<square> (GLfloat(stof(begin[0])));
}

shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 2) throw runtime_error
            ("make_diamond: invalid number of args. Need 1.");
   return make_shared<diamond> (GLfloat(stof(begin[0])),
            GLfloat(stof(begin[1])));
}

shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(((end - begin) % 2 ) == 1 or (end - begin) != 6) throw
        runtime_error("make_triangle: invalid number of args. Need 6.");
   vertex_list vert_list{};
   for (auto itor = begin; itor != end; ++itor) {
      vertex vert{stof(*itor++), stof(*itor)};
      vert_list.push_back(vert);
   }
   return make_shared<triangle> (vert_list);
}

shape_ptr interpreter::make_equilateral (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if((end - begin) != 1) throw runtime_error
            ("make_equilateral: invalid number of args. Need 1.");

   return make_shared<equilateral> (GLfloat(stof(begin[0])));
}
