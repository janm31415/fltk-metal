#import <Cocoa/Cocoa.h>
#include <FL/x.H>
#include <FL/Fl_Window.H>

#import <MetalKit/MetalKit.h>

#include "metal_window.h"

metal_window::metal_window(int x, int y, int w, int h, const char* t) : Fl_Window(x, y, w, h, t),
_initialised(false)
{
  box(FL_NO_BOX);
}

metal_window::~metal_window()
{
}

void metal_window::prepare()
{
  NSView *view = [fl_xid(this) contentView];
  NSRect f = view.frame;
  view.layer = [CAMetalLayer layer];
  view.layer.frame = f;
  CAMetalLayer* metal_layer = (CAMetalLayer*)view.layer;
  metal_layer.device = ::MTLCreateSystemDefaultDevice();
  MTL::Device* device = ( __bridge MTL::Device* )metal_layer.device;
  _initialised = true;
}

void metal_window::draw()
{
  if (!_initialised)
    prepare();
  NSView *view = [fl_xid(this) contentView];
  CAMetalLayer* metal_layer = (CAMetalLayer*)view.layer;
  id<CAMetalDrawable> currentDrawable = [metal_layer nextDrawable];
  CA::MetalDrawable* pMetalCppDrawable  = ( __bridge CA::MetalDrawable* ) currentDrawable;
  _draw(( __bridge MTL::Device* )metal_layer.device, pMetalCppDrawable);
}

void metal_window::redraw()
  {
  draw();
  }
