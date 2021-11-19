#ifndef CANVAS_H_
#define CANVAS_H_

#include "Grid.h"

namespace GLOO {
class Color {
public:
  Color();
  Color(float red, float green, float blue);
  ~Color();
  float r,g,b;
  Color& operator+=(Color& c){
    r+=c.r;
    g+=c.g;
    b+=c.b;
    return *this;
  };
  Color& operator*(float& s){
    Color c=*this;
    c.r *=s;
    c.g *=s;
    c.b *=s;
  };
private:
};

class Canvas {
public:
  Canvas(int width, int height);
  ~Canvas();
  void drawLine(int from_x, int from_y, int to_x, int to_y);
  void drawPoint(int pos_x, int pos_y, int size);
  void fillRectangle(BBox<int> rect);
  void setPixel(int i, int j, Color c);
  void addToPixel(int i, int j, Color c);
  void fill(Color c);
private:
  const int _width;
  const int _height;
  Color line_color_;
  Color fill_color_;
  Grid<Color> pixel_data_;
};


}  // namespace GLOO

#endif
