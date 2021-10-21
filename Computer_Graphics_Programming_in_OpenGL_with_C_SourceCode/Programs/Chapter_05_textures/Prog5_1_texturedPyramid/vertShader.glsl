#version 430
#define M_PI 3.1415
layout (location=0) in vec3 pos;
layout (location=1) in vec2 texCoord;

uniform float tf;

out vec2 tc;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
layout (binding=0) uniform sampler2D samp;

mat4 buildRotateY(float rad);

void main(void)
{	
	gl_Position = proj_matrix * mv_matrix * buildRotateY(tf*M_PI*10/180.0f) * vec4(pos,1.0);
	tc = texCoord;
}

mat4 buildRotateY(float rad)
{	mat4 yrot = mat4(	cos(rad), 0.0, sin(rad), 0.0,
		0.0, 1.0, 0.0, 0.0,
		-sin(rad), 0.0, cos(rad), 0.0,
		0.0, 0.0, 0.0, 1.0 );
	return yrot;
}
