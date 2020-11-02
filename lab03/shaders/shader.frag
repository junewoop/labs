#version 330 core

uniform mat4 model, view, projection;

in vec4 WorldSpace_position;
in vec4 WorldSpace_normal;

// Light properties
const vec3 WorldSpace_lightPos = vec3(2, 2, 2); // world-space light position
uniform vec3 lightColor;
uniform float lightIntensity;

//uniform vec4 normalVector;


//vec3 normalVector;


// Attenuation Properties
uniform float attQuadratic;
uniform float attLinear;
uniform float attConstant;

// Material properties
uniform vec3 color;
uniform float ambientIntensity;
uniform float diffuseIntensity;
uniform float specularIntensity;
uniform float shininess;


out vec3 fragColor;

void main(){
    vec4 normalVector = normalize(WorldSpace_normal);
    vec4 lightVector = normalize(vec4(WorldSpace_lightPos, 1.0) - WorldSpace_position);
    vec4 eyePos = normalize(inverse(view)*vec4(WorldSpace_lightPos, 1.0));
    vec4 reflectVector = reflect(-lightVector, normalVector);
    float d = distance(vec4(WorldSpace_lightPos, 1.0), WorldSpace_position);
    float lightMultiplier= min(1/(attConstant + attLinear * d + attQuadratic * d * d), 1.0f)*lightIntensity;

    fragColor = ambientIntensity * color;
    fragColor += lightMultiplier*(lightColor * diffuseIntensity * max(dot(normalize(WorldSpace_normal), normalize(vec4(WorldSpace_lightPos, 1.0) - WorldSpace_position)), 0))*color;
    fragColor += (lightMultiplier * lightColor * specularIntensity * pow(max(dot(eyePos, reflectVector), 0.0), shininess))*color;

}
