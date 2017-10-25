#version 120

uniform float time = 1.0;
uniform vec2 mouse;
uniform vec2 resolution;



void main(void)
{
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    
    //get our current vertex position so we can modify it
    vec4 pos = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
    
    vec2 m = mouse - resolution/2.0;
    
//    vec2 distToCenter = distance(vec2( pos.x, pos.y), resolution/2.0);
    float mouseToVert = length(distance(m, pos.xy));
    
    if( mouseToVert < 100 ){
        
        pos.y += 100;
        
        gl_FrontColor = vec4(1.0, 0.0, 0.0, 1.0);
        
    } else {
        gl_FrontColor = gl_Color;
    }
 
    //finally set the pos to be that actual position rendered
    gl_Position = pos;
    
    
}
