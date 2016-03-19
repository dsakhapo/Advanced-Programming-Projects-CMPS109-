// $Id: shape.cpp,v 1.1 2015-07-16 16:47:51-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#include <typeinfo>
#include <unordered_map>
#include <cmath>
using namespace std;

#include "shape.h"
#include "util.h"
#include "graphics.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

static unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};

string find_fontname(void* func_font){
   auto i = fontname.find(func_font);
   return i->second;
}

void* find_fontcode(string font_name){
   auto i = fontcode.find(font_name);
   return i->second;
}

void* search_fontcode(const string& font_name){
   auto i = fontcode.find(font_name);
   if(i == fontcode.end()) throw runtime_error("invalid font");
   return i->second;
}

ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}

text::text (void* glut_bitmap_font, const string& textdata):
      glut_bitmap_font(glut_bitmap_font), textdata(textdata) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}

//Polygon takes a list of vertices and
//connects them together in list order.
polygon::polygon (const vertex_list& vertices): vertices(vertices) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height):
            polygon({{0, 0}, {width, 0},
   {width, height}, {0, height}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this);
}

diamond::diamond(const GLfloat width, const GLfloat height) :
         polygon(
                  { { 0, GLfloat(.5) * height }, { GLfloat(.5) * width,
                           0 }, { width, GLfloat(.5) * height }, {
                           GLfloat(.5) * width, height } }) {
   DEBUGF('c', this);
}

triangle::triangle(const vertex_list& vertices): polygon(vertices) {
   DEBUGF ('c', this);
}


equilateral::equilateral(GLfloat width) :
         polygon(
                  { { 0, 0 }, { width, 0 }, { width / 2, width
                           * GLfloat(0.866025404) } }) {
   // .866025404*width is the relative scalar height for an equilateral
   // triangle knowing the length of its sides. It was computed via
   // Pythagorian Theorem.
   DEBUGF('c', this);
}

void text::draw (const vertex& center, const rgbcolor& color) const {
   //Highlight all of the text if its selected
   glColor3ubv(window::get_draw_border() ?
            rgbcolor(window::get_color()).ubvec :
            color.ubvec);
   glRasterPos2f(center.xpos, center.ypos);
   glutBitmapString(glut_bitmap_font,
            reinterpret_cast<const GLubyte*>(textdata.c_str()));
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void ellipse::draw (const vertex& center, const rgbcolor& color) const {
   glBegin(GL_POLYGON);          //Interpret vertices as GL_POLYGON
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);     //Specify the RGBA color of the ellipse
   const float delta = 2 * M_PI / 32;
   for(float theta = 0; theta < 2 * M_PI; theta += delta){
      float xpos = dimension.xpos/2 * cos(theta) + center.xpos;
      float ypos = dimension.ypos/2 * sin(theta) + center.ypos;
      glVertex2f(xpos, ypos);    //Specify polygon vertices (x,y)
   }
   glEnd();

   if (window::get_draw_border()) {
      glLineWidth(window::get_thickness());
      glBegin(GL_LINE_LOOP);
      glEnable(GL_LINE_SMOOTH);
      glColor3ubv(rgbcolor(window::get_color()).ubvec);
      for (float theta = 0; theta < 2 * M_PI; theta += delta) {
         float xpos = dimension.xpos / 2 * cos(theta) + center.xpos;
         float ypos = dimension.ypos / 2 * sin(theta) + center.ypos;
         glVertex2f(xpos, ypos);
      }
      glEnd();
   }
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void polygon::draw (const vertex& center, const rgbcolor& color) const {
   float xavg = 0; //Need the x and y average to calculate
   float yavg = 0; //the center of the polygon
   for(const auto& vertex: vertices){
      xavg += vertex.xpos;
      yavg += vertex.ypos;
   }
   xavg /= vertices.size(); yavg /= vertices.size();
   glBegin(GL_POLYGON);
   glEnable(GL_LINE_SMOOTH);
   glColor3ubv(color.ubvec);
   for(auto itor = vertices.cbegin(); itor != vertices.cend(); ++itor){
      float xpos = center.xpos + itor->xpos - xavg;
      float ypos = center.ypos + itor->ypos - yavg;
      glVertex2f(xpos, ypos);
   }
   glEnd();

   if(window::get_draw_border()){
      glLineWidth(window::get_thickness());
      glBegin(GL_LINE_LOOP);
      glEnable(GL_LINE_SMOOTH);
      glColor3ubv(rgbcolor(window::get_color()).ubvec);
      for (auto itor = vertices.cbegin(); itor != vertices.cend();
               ++itor) {
         float xpos = center.xpos + itor->xpos - xavg;
         float ypos = center.ypos + itor->ypos - yavg;
         glVertex2f(xpos, ypos);
      }
      glEnd();
   }
   DEBUGF ('d', this << "(" << center << "," << color << ")");
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": ";
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

