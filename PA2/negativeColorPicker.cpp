/*
*  File: negativeColorPicker.cpp
*  Implements the negative color picker for CPSC 221 PA2.
*
*/

#include "negativeColorPicker.h"

NegativeColorPicker::NegativeColorPicker(PNG& inputimg)
{
  img = inputimg;  
}

HSLAPixel NegativeColorPicker::operator()(PixelPoint p)
{
  unsigned int x = p.x % img_other.width();
  unsigned int y = p.y % img_other.height();
  
  HSLAPixel * pixel = img_other.getPixel(x,y);

  pixel->l = 1.0 - pixel->l;

  pixel->h += 180;
  if (pixel->h > 360) {
    pixel->h -= 180;
  }

  return *pixel;
}
