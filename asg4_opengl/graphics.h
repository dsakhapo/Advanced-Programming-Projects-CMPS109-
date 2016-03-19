// $Id: graphics.h,v 1.1 2015-07-16 16:47:51-07 - - $
// Partner: Darius Sakhapour(dsakhapo@ucsc.edu)
// Partner: Ryan Wong (rystwong@ucsc.edu)
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"

class object {
   private:
      shared_ptr<shape> pshape;
      vertex center;
      rgbcolor color;
      bool selected = false;
   public:
      // Default copiers, movers, dtor all OK.
      object();
      object(const vertex& v, const rgbcolor& r, shared_ptr<shape> s):
         pshape(s), center(v), color(r)
         {}
      void draw() { pshape->draw (center, color); }
      void move (GLfloat delta_x, GLfloat delta_y) {
         center.xpos += delta_x;
         center.ypos += delta_y;
      }
      void set_selected(bool select) { selected = select; }
      bool get_selected() { return selected; }
      vertex get_center() { return center; }
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};

class window {
      friend class mouse;
   private:
      static int width;         // in pixels
      static int height;        // in pixels
      static vector<object> objects;
      static size_t selected_obj;
      static bool draw_border;
      static mouse mus;
      static int obj_speed;
      static int thickness;
      static string color;
      static bool first_run;
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
      static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
   public:
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      static void main();
      // Object Selection Functions
      static void select_object_next();
      static void select_object_prev();
      static void select_object(size_t obj);
      static void draw_num(object, size_t);
      // Object Movement Functions
      static void move_selected_object(int, int);
      static void moveby(int speed) {obj_speed = speed;}
      static int get_obj_speed() {return obj_speed;}
      static void border(const string& c, int t)
         { color = c; thickness = t; }
      // Getters
      static vector<object> get_objects (){ return objects; }
      static size_t get_selected_obj() { return selected_obj; }
      static int get_thickness() { return thickness; }
      static string get_color() { return color; }
      static bool get_draw_border(){ return draw_border; }
      static void set_draw_border(bool border){ draw_border = border; }
      static bool get_first_run(){ return first_run; }
      static void set_first_run(bool run){ first_run = run; }
};

#endif

