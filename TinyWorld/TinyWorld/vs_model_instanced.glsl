#version 430 core
#define MAX_INSTANCES 55

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 tc;
layout (location = 2) in vec4 normals;

out VS_OUT
{
    vec2 tc;
    vec4 normals;
    vec4 fragPos;
} vs_out;

uniform mat4 view_matrix;
uniform mat4 proj_matrix;
uniform mat4 matArray[MAX_INSTANCES]; //model matrix array 


void main(void)
{
	// calculate position for each object instance based on its own model matrix
    gl_Position = proj_matrix * view_matrix * matArray[gl_InstanceID] * position;
    vs_out.tc = tc;

    vec3 normalsT = mat3(transpose(inverse(matArray[gl_InstanceID]))) * vec3(normals.xyz);
    vs_out.normals = vec4(normalsT, 1.0);

    vs_out.fragPos = matArray[gl_InstanceID] * position;
}
