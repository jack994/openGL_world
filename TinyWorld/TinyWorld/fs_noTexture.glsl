#version 430 core

out vec4 color;

in VS_OUT
{
    vec2 tc;
    vec4 normals;
    vec4 fragPos;
} fs_in;

layout(binding=0) uniform sampler2D tex;

uniform mat4 model_matrix;

uniform vec4 lightPosition;
uniform vec4 lightPositionT;
uniform vec4 viewPosition;

uniform vec3 la;		// Ambient colour
uniform vec4 ka;		// Ambient constant
uniform vec3 ld;		// diffuse colour
uniform vec3 ldT;		// diffuse colour
uniform vec4 kd;		// Diffuse constant
uniform vec3 ls;		// specular colour
uniform vec3 lsT;		// specular colour
uniform vec4 ks;		// specular constant
uniform float shininess;// shininess constant

uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;

uniform vec4 lightSpotDirection;
uniform float lightSpotCutOff;
uniform float lightSpotOuterCutOff;

uniform float onoff;


void main(void){

	vec4 lightDir = normalize(lightPositionT - fs_in.fragPos);
 
	// Ambient 
	vec3 ambient = ka.rgb * la;


	// Diffuse	
	float diff = max(dot(normalize(fs_in.normals), lightDir), 0.0);
	vec3 diffuse = kd.rgb * ldT * diff;

	// Specular
	vec4 viewDir = normalize(viewPosition - fs_in.fragPos);
	vec4 reflectDir = reflect(-lightDir, normalize(fs_in.normals));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = ks.rgb * lsT * spec;

	 // Attenuation
    float distance    = length(lightPosition - fs_in.fragPos);
    float attenuation = 1.0f / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));  

	ambient  *= attenuation;  
	diffuse  *= attenuation * onoff;
	specular *= attenuation * onoff;   

	// Spot light
	float theta = dot(lightDir, normalize(-lightSpotDirection));
    float epsilon = (lightSpotCutOff - lightSpotOuterCutOff);
    float intensity = clamp((theta - lightSpotOuterCutOff) / epsilon, 0.0, 1.0);

    diffuse  *= intensity;
	specular *= intensity;   




	vec4 lightDir2 = normalize(lightPosition - fs_in.fragPos);

	//diffuse
	float diff2 = max(dot(normalize(fs_in.normals), lightDir2), 0.0);
	vec3 diffuse2 = kd.rgb * ld * diff2;

	// Specular
	vec4 viewDir2 = normalize(viewPosition - fs_in.fragPos);
	vec4 reflectDir2 = reflect(-lightDir2, normalize(fs_in.normals));
	float spec2 = pow(max(dot(viewDir2, reflectDir2), 0.0), shininess);
	vec3 specular2 = ks.rgb * ls * spec2;

	
	

	// Light
	color = vec4(ambient + diffuse + diffuse2 + specular + specular2, 1.0); 
}


 