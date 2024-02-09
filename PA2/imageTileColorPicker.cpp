/*
*  File: imageTileColorPicker.cpp
*  Implements the image tile color picker for CPSC 221 PA2.
*
*/

#include "imageTileColorPicker.h"

ImageTileColorPicker::ImageTileColorPicker(PNG& otherimage) {
  img_other = otherimage;
  
}

HSLAPixel ImageTileColorPicker::operator()(PixelPoint p) {
  unsigned int x = p.x % img_other.width();
  unsigned int y = p.y % img_other.height();
  
  HSLAPixel * pixel = img_other.getPixel(x,y);
  
  return **pixel;
}