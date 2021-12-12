#include <metal_stdlib>
using namespace metal;

struct VertexIn {
  packed_float3 position;
  packed_float3 normal;
  packed_float2 textureCoordinates;
};

struct PoseInfo {
  float4x4 pose;
  float4x4 projection_matrix;
};

struct VertexOut {
  float4 position [[position]];
  float3 normal;
  float2 texcoord;
};

vertex VertexOut vertex_shader(const device VertexIn *vertices [[buffer(0)]], uint vertexId [[vertex_id]], constant PoseInfo& input [[buffer(1)]]) {
  float4 pos(vertices[vertexId].position, 1);
  VertexOut out;
  out.position = input.projection_matrix * input.pose * pos;
  out.normal = (input.pose * float4(vertices[vertexId].normal, 0)).xyz;
  out.texcoord = vertices[vertexId].textureCoordinates;
  return out;
}


fragment float4 fragment_shader(const VertexOut vertexIn [[stage_in]], texture2d<float> texture [[texture(0)]], sampler sampler2d [[sampler(0)]]) {
  float Ambient = 0.2;
  float3 LightDir = normalize(float3(0.2, 0.3, 0.4));
  float l = clamp(dot(vertexIn.normal,LightDir), 0.0, 1.0 - Ambient) + Ambient;
  return texture.sample(sampler2d, vertexIn.texcoord)*l;
}
