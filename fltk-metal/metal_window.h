#ifndef METAL_WINDOW_H
#define METAL_WINDOW_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>

#include "metal/Metal.hpp"

class metal_window : public Fl_Window
{
public:
  explicit metal_window(int x, int y, int w, int h, const char* t);
  virtual ~metal_window();
  
  void prepare();
  
  void draw();
  
  virtual void redraw();
  
private:
  
  virtual void _draw(MTL::Device* device, CA::MetalDrawable* drawable) = 0;
  
private:
  bool _initialised;
};

#endif
