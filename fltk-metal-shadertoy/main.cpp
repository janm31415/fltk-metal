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

struct PoseInfo
{
  float proj_matrix[16];
};

struct ShaderInfo
{
  float resolution_x, resolution_y;
  float time;
};

class metal_canvas : public Fl_Metal_Window
{
public:
  metal_canvas(int x, int y, int w, int h, const char* t) : Fl_Metal_Window(x, y, w, h, t),
  mp_device(nullptr), mp_vertex_buffer(nullptr), mp_vertex_index_buffer(nullptr),
  mp_descriptor(nullptr), mp_command_queue(nullptr), mp_material_pipeline(nullptr)
  {
    m_in_flight_semaphore = dispatch_semaphore_create(1);
    resizable(this);
    _pose_info.proj_matrix[0] = 1;
    _pose_info.proj_matrix[1] = 0;
    _pose_info.proj_matrix[2] = 0;
    _pose_info.proj_matrix[3] = 0;
    _pose_info.proj_matrix[4] = 0;
    _pose_info.proj_matrix[5] = 1;
    _pose_info.proj_matrix[6] = 0;
    _pose_info.proj_matrix[7] = 0;
    _pose_info.proj_matrix[8] = 0;
    _pose_info.proj_matrix[9] = 0;
    _pose_info.proj_matrix[10] = 0;
    _pose_info.proj_matrix[11] = -1;
    _pose_info.proj_matrix[12] = 0;
    _pose_info.proj_matrix[13] = 0;
    _pose_info.proj_matrix[14] = 0;
    _pose_info.proj_matrix[15] = 1;
    
    _shader_info.resolution_x = (float)w;
    _shader_info.resolution_y = (float)h;
    
    _start = std::chrono::high_resolution_clock::now();
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
    
    float vertices[12] = {-1.f,-1.f,0.f,1.f,-1.f,0.f,1.f,1.f,0.f,-1.f,1.f,0.f};
    uint32_t indices[6] = {0,1,2,0,2,3};
    
    MTL::ResourceOptions options = 0;
    mp_vertex_buffer = mp_device->newBuffer((const void*)vertices, sizeof(float)*12, options);
    mp_vertex_index_buffer = mp_device->newBuffer((const void*)indices, 6*sizeof(uint32_t), options);
    
    MTL::Library* lib = mp_device->newDefaultLibrary();
    NS::String* vf_name = NS::String::string("vertex_shader", NS::UTF8StringEncoding);
    MTL::Function* vertex_function = lib->newFunction(vf_name);
    NS::String* ff_name = NS::String::string("fragment_shader", NS::UTF8StringEncoding);
    MTL::Function* fragment_function = lib->newFunction(ff_name);
    
    MTL::RenderPipelineDescriptor* descr = MTL::RenderPipelineDescriptor::alloc()->init();
    descr->setVertexFunction(vertex_function);
    descr->setFragmentFunction(fragment_function);
    descr->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
    descr->setDepthAttachmentPixelFormat(MTL::PixelFormatInvalid);
    
    NS::Error* err;
    mp_material_pipeline = mp_device->newRenderPipelineState(descr, &err);
    
    descr->release();
    lib->release();
  }
  
  virtual void _draw(MTL::Device* device, CA::MetalDrawable* drawable)
  {
    if (!mp_device)
      _init_engine(device, drawable);
    
    dispatch_semaphore_wait(this->m_in_flight_semaphore, DISPATCH_TIME_FOREVER);
    
    
    MTL::CommandBuffer* commandBuffer = mp_command_queue->commandBuffer();
    commandBuffer->addCompletedHandler([&](MTL::CommandBuffer* buf){dispatch_semaphore_signal(this->m_in_flight_semaphore);});
    
    _shader_info.time = (float)(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _start).count()) / 1000000.f;
    
    mp_descriptor->colorAttachments()->object(0)->setTexture(drawable->texture());
    MTL::RenderCommandEncoder* drawOnScreenCommandEncoder = commandBuffer->renderCommandEncoder(mp_descriptor);
     drawOnScreenCommandEncoder->setRenderPipelineState(mp_material_pipeline);
    drawOnScreenCommandEncoder->setVertexBuffer(mp_vertex_buffer, 0, 0);
    drawOnScreenCommandEncoder->setVertexBytes(&_pose_info, sizeof(PoseInfo), 1);
    drawOnScreenCommandEncoder->setFragmentBytes(&_shader_info, sizeof(ShaderInfo), 1);
    drawOnScreenCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, 6, MTL::IndexTypeUInt32, mp_vertex_index_buffer, 0);
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
  PoseInfo _pose_info;
  ShaderInfo _shader_info;
  std::chrono::steady_clock::time_point _start;
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
  
  virtual void flush()
  {
    _metal_canvas->flush();
    Fl_Double_Window::flush();
  }
  
private:
  metal_canvas* _metal_canvas;
};


int main(int argc, char** argv)
{
  int result = -1;
  fltk_metal_window* window = new fltk_metal_window(100, 100, 800, 450, "fltk-metal-shadertoy");
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
