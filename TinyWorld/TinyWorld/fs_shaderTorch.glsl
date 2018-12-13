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

uniform vec3 la;		// Ambient light colour
uniform vec4 ka;		// Ambient color material
uniform vec3 ld;		// diffuse colour sun
uniform vec3 ldT;		    // diffuse colour torch
uniform vec4 kd;		    // Diffuse color material
uniform vec3 ls;		    // specular colour sun
uniform vec3 lsT;		    // specular colour torch
uniform vec4 ks;		    // specular color material
uniform float shininess;        // shininess material

uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;
uniform vec4 lightSpotDirection;
uniform float lightSpotCutOff;
uniform float lightSpotOuterCutOff;
uniform float onoff; //torch on or off

void main(void){
	vec4 lightDir = normalize(lightPositionT - fs_in.fragPos); //torch
	vec4 lightDir2 = normalize(lightPosition - fs_in.fragPos); //moon
 
	// Ambient (used only once) 
	vec3 ambient = la * texture(tex, fs_in.tc).rgb;

	// Diffuse torch	
	float diff = max(dot(normalize(fs_in.normals), lightDir), 0.0);
	vec3 diffuse = kd.rgb * ldT * diff * texture(tex, fs_in.tc).rgb;

	//diffuse moon
	float diff2 = max(dot(normalize(fs_in.normals), lightDir2), 0.0);
	vec3 diffuse2 =  ld * kd.rgb * diff2 * texture(tex, fs_in.tc).rgb;

	// Specular torch
	vec4 viewDir = normalize(viewPosition - fs_in.fragPos);
	vec4 reflectDir = reflect(-lightDir, normalize(fs_in.normals));
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = ks.rgb * lsT * spec;

	// Specular moon
	vec4 viewDir2 = normalize(viewPosition - fs_in.fragPos);
	vec4 reflectDir2 = reflect(-lightDir2, normalize(fs_in.normals));
	float spec2 = pow(max(dot(viewDir2, reflectDir2), 0.0), shininess);
	vec3 specular2 = ls * ks.rgb * spec2;

	// Attenuation torch
    	float distance    = length(lightPositionT - fs_in.fragPos);
    	float attenuation = 1.0f / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));  

	// Attenuation moon
    	float distance2    = length(lightPosition - fs_in.fragPos);
    	float attenuation2 = 1.0f / (lightConstant + lightLinear * distance2 + lightQuadratic * (distance2 * distance2)); 

	// Spot light torch
	float theta = dot(lightDir, normalize(-lightSpotDirection));
    	float epsilon = (lightSpotCutOff - lightSpotOuterCutOff);
    	float intensity = clamp((theta - lightSpotOuterCutOff) / epsilon, 0.0, 1.0);

	ambient  *= attenuation2;
  
	diffuse  *= attenuation * intensity * onoff; //check torch is on or off
	specular *= attenuation * intensity * onoff;  
	diffuse2  *= attenuation2;
	specular2 *= attenuation2; 

	// Final Color adding lights
	color = vec4(ambient + diffuse + diffuse2 + specular + specular2, 1.0); 
}


 