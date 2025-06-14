#version 330 core

struct Spotlight {
    vec3 position;
    vec3 direction;
    float cutoff;

    vec3 diffuseColor;
    vec3 ambientColor;
};

#define NUM_SPOTLIGHTS 3

uniform sampler2D textureT;
uniform sampler2D texture2;
uniform sampler2D texture3;
uniform int shapeID;

//uniform vec3 lightPos = vec3(0.0,200.0,0.0);
//uniform vec3 diffuseColor = vec3(1.0,1.0,1.0);
//uniform vec3 ambientColor = vec3(0.2,0.2,0.2);
//uniform vec3 viewPos;
uniform float kc = 1;
uniform float kl = 0.35 * 1e-4;
uniform float kq = 0.44 * 1e-4;

uniform Spotlight spotlights[NUM_SPOTLIGHTS];
uniform int numSpotlights;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 FragPos);

void main(){
    
    vec3 norm = normalize(Normal);
    //vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    for(int i = 0; i < NUM_SPOTLIGHTS; i++){
        result += CalcSpotLight(spotlights[i], norm, FragPos);
    }

    //FragColor = vec4(result, 1.0);    

    // Output the color based on the shape ID
    if (shapeID == 0) {
        FragColor = vec4(result * texture(textureT, TexCoord).rgb, 1.0);
    } else if (shapeID == 1) {
        FragColor = vec4(result * texture(texture2, TexCoord).rgb, 1.0);
    } else if (shapeID == 2) {
        FragColor = vec4(result * texture(texture3, TexCoord).rgb, 1.0);
    } else {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Default color if shape ID is not recognized
    } 

}

vec3 CalcSpotLight(Spotlight light, vec3 normal, vec3 FragPos)
{
        vec3 lightDir = normalize(light.position - FragPos);

        // cutoff
        float theta = dot(lightDir, normalize(-light.direction));

        if (theta > light.cutoff) {
            
            // diffuse shading
            vec3 norm = normalize(Normal);
            float diff = max(dot(norm, lightDir), 0.0);

            // attenuation
            float distance = length(light.position - FragPos);
            float attenuation = 1.0 / (kc + kl * distance + kq * distance * distance); // attenuation based on distance  

            // combine results
            vec3 ambient = light.ambientColor;
            vec3 diffuse = diff * light.diffuseColor * attenuation;

            return (ambient + diffuse);
        }

        else {
            return (light.ambientColor);
        }
}