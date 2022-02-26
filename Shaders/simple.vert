#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 FlatNormal;
layout(location = 3) in vec3 SmoothNormal;
out vec4 vColor;

uniform mat4 ModelToWorld;
uniform mat4 WorldToView;
uniform int Shading;
void main()
{
  vec3 dir = vec3(0.7071067811865475, 0, -0.7071067811865475);

  gl_Position = WorldToView * ModelToWorld * vec4(position, 1.0);
  float diffuse = .5 + dot(Shading == 0 ? FlatNormal : SmoothNormal ,dir);
  vColor = vec4(diffuse * color, 1.0);
}
