#version 430

in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;
in vec3 varyingHalfVector;

out vec4 fragColor;

struct PositionalLight
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	vec3 position;
};

struct SpotLight
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	vec3 location;
	vec3 direction;
	float cutoff;
	float outcutoff;
	float exponent;
};

struct Material
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform SpotLight slight;
uniform Material material;
uniform mat4 mv_matrix;	 
uniform mat4 proj_matrix;
uniform mat4 norm_matrix;

void main(void)
{	// normalize the light, normal, and view vectors:
	vec3 L = normalize(varyingLightDir);
	vec3 N = normalize(varyingNormal);
	vec3 V = normalize(-varyingVertPos);
	
	// get the angle between the light and surface normal:
	float cosTheta = dot(L,N);
	float cosPhi = dot(normalize(slight.direction), -1*L);
	//float theta = dot(lightDirection, normalize(SpotDir));
	float epsilon = slight.cutoff - slight.outcutoff;
	float intensity = clamp( ( cosPhi - slight.outcutoff ) / epsilon, 0.0, 1.0 );
	// halfway vector varyingHalfVector was computed in the vertex shader,
	// and interpolated prior to reaching the fragment shader.
	// It is copied into variable H here for convenience later.
	vec3 H = normalize(varyingHalfVector);
	
	// get angle between the normal and the halfway vector
	float cosGam = dot(H,N);

	// compute ADS contributions (per pixel):
	vec3 ambient = ((globalAmbient * material.ambient) + (slight.ambient * material.ambient)).xyz;
	vec3 diffuse = slight.diffuse.xyz * material.diffuse.xyz * max(cosTheta,0.0);
	vec3 specular = slight.specular.xyz * material.specular.xyz * pow(max(cosGam,0.0), material.shininess*3.0);
	intensity = pow(max(intensity,0.0), slight.exponent);   

	fragColor = vec4(min(ambient + diffuse * intensity + specular * intensity, vec3(1.0)), 1.0);
	//fragColor = vec4(diffuse * intensity, 1.0);
}
