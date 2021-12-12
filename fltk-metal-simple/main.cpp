#include <stdio.h>
#include <string.h>
#include <iostream>

// In exactly one cpp file we create the CPP implementation for Metal.hpp
// See https://github.com/bkaradzic/metal-cpp
#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#include "metal/Metal.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "fltk-metal/Fl_Metal_Window.h"

#include <iostream>
#include <chrono>

#include <vector>


class metal_canvas : public Fl_Metal_Window
{
public:
  metal_canvas(int x, int y, int w, int h, const char* t) : Fl_Metal_Window(x, y, w, h, t),
  mp_device(nullptr), mp_vertex_buffer(nullptr), mp_vertex_index_buffer(nullptr),
  mp_descriptor(nullptr), mp_command_queue(nullptr), mp_material_pipeline(nullptr)
  {
    m_in_flight_semaphore = dispatch_semaphore_create(1);
    resizable(this);
  }
  
  virtual ~metal_canvas()
  {
    if (mp_vertex_buffer)
      mp_vertex_buffer->release();
    if (mp_vertex_index_buffer)
      mp_vertex_index_buffer->release();
    if (mp_descriptor)
      mp_descriptor->release();
    if (mp_command_queue)
      mp_command_queue->release();
    if (mp_material_pipeline)
      mp_material_pipeline->release();
  }
  
  
private:
  
  void _init_engine(MTL::Device* device, CA::MetalDrawable* drawable)
  {
    mp_device = device;
    mp_command_queue = mp_device->newCommandQueue();
    mp_descriptor = MTL::RenderPassDescriptor::alloc()->init();
    mp_descriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(0.12, 0.38, 0.85, 1));
    mp_descriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
  }
  
  virtual void _draw(MTL::Device* device, CA::MetalDrawable* drawable)
  {
    if (!mp_device)
      _init_engine(device, drawable);
    
    dispatch_semaphore_wait(this->m_in_flight_semaphore, DISPATCH_TIME_FOREVER);
    
    
    MTL::CommandBuffer* commandBuffer = mp_command_queue->commandBuffer();
    commandBuffer->addCompletedHandler([&](MTL::CommandBuffer* buf){dispatch_semaphore_signal(this->m_in_flight_semaphore);});
    
    mp_descriptor->colorAttachments()->object(0)->setTexture(drawable->texture());
    MTL::RenderCommandEncoder* drawOnScreenCommandEncoder = commandBuffer->renderCommandEncoder(mp_descriptor);
    drawOnScreenCommandEncoder->endEncoding();
  
    commandBuffer->presentDrawable(drawable);
    commandBuffer->commit();
  }
  
private:
  MTL::Device* mp_device;
  dispatch_semaphore_t m_in_flight_semaphore;
  MTL::Buffer* mp_vertex_buffer;
  MTL::Buffer* mp_vertex_index_buffer;
  MTL::RenderPassDescriptor* mp_descriptor;
  MTL::CommandQueue* mp_command_queue;
  MTL::RenderPipelineState* mp_material_pipeline;
};

class fltk_metal_window : public Fl_Double_Window
{
public:
  fltk_metal_window(int x, int y, int w, int h, const char* t) : Fl_Double_Window(x, y, w, h, t)
  {
    box(FL_NO_BOX);
    _metal_canvas = new metal_canvas(0, 0, this->w(), this->h(), t);
  }
  
  virtual ~fltk_metal_window()
  {
    delete _metal_canvas;
  }
  
  virtual void redraw()
  {
    _metal_canvas->redraw();
    Fl_Double_Window::redraw();
  }
  
private:
  metal_canvas* _metal_canvas;
  
};


int main(int argc, char** argv)
{
  int result = -1;
  fltk_metal_window* window = new fltk_metal_window(100, 100, 800, 450, "fltk-metal");
  window->end();
  window->show(argc, argv);
  while (Fl::check())
  {
    window->redraw();
    Fl::wait(0.016);
  }
  delete window;
  return 0;
}
