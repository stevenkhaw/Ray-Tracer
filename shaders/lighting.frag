#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

// Material parameters
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;

// Light source parameters
const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

// Output the frag color
out vec4 fragColor;


void main (void){
    if (!enablelighting){
        // Default normal coloring (you don't need to modify anything here)
        vec3 N = normalize(normal);
        fragColor = vec4(0.5f*N + 0.5f , 1.0f);
    } else {
        
        // HW3: You will compute the lighting here.

        //lets convert everything into our camera coordinates
        vec3 lj ;
        vec4 camPos = modelview * position;
        //to get vector towards camera, we do xcamPos = ((0,0,0,1) - camPos).xyz  this vector then needs to be normalized
        vec3 v = normalize(vec4(0,0,0,1) - camPos).xyz;
        //normalize... this is probably the source of issues because we really only need to invert and transpose over the inner 3x3 matrix
        vec3 n = normalize(transpose(inverse(mat3(modelview))) * normalize(normal));


        //we are going to do a loop, where each iteration goes over one light
        vec4 total = vec4(0, 0 , 0, 0);
        vec4 iterationSum; 

        for(int i = 0; i < nlights; i++){
            // only care about xyz components of lj
            vec4 test = vec4(0,0,0,1);
            vec4 lgb = view*lightpositions[i];
            lj = normalize(test.w*lgb.xyz - lgb.w*test.xyz).xyz ;
            iterationSum =  vec4(0, 0 , 0, 0);

            //ambient
            iterationSum += ambient;

            //diffuse
            iterationSum += diffuse * max(dot(n, lj),0);

            //specular
            iterationSum += specular*pow(max(dot(n, normalize(v+lj)),0),shininess);

            //all together, we multiply this by the light color
            total += iterationSum * lightcolors[i];
        }
        total += emision;
        fragColor = total;
    }
}
