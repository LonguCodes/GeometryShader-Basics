#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 outColor[];

out vec3 vColor;

void main() {    

    vColor =(outColor[0]+outColor[1]+outColor[2])/3;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;

    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    
    EmitVertex();
    
    EndPrimitive();
}  