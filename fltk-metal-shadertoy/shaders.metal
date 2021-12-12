#include <metal_stdlib>
using namespace metal;

struct VertexIn {
  packed_float3 position;
};

struct PoseInfo {
  float4x4 projection_matrix;
};

struct VertexOut {
  float4 position [[position]];
};

vertex VertexOut vertex_shader(const device VertexIn *vertices [[buffer(0)]], uint vertexId [[vertex_id]], constant PoseInfo& input [[buffer(1)]]) {
  float4 pos(vertices[vertexId].position, 1);
  VertexOut out;
  out.position = input.projection_matrix * pos;
  return out;
}

struct ShaderInfo {
  packed_float2 iResolution;
  float iTime;
};

fragment float4 fragment_shader(const VertexOut vertexIn [[stage_in]], constant ShaderInfo& info [[buffer(1)]]) {
  // Normalized pixel coordinates (from 0 to 1)
  float2 uv = vertexIn.position.xy/info.iResolution.xy;
        
  // Time varying pixel color
  float3 col = 0.5 + 0.5*cos(info.iTime+uv.xyx+float3(0,2,4));
  
  // Output to screen
  return float4(col, 1);

}
