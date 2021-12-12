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

fragment float4 fragment_shader(const VertexOut vertexIn [[stage_in]]) {
  return float4(1, 0, 0, 1);
}
