#version 120


uniform float time;
uniform float fogLayers;
uniform float driftSpeed;
uniform float noiseSpeed;
uniform float noiseScale;
uniform float ySquish;
uniform float octaveDiff;
uniform float fogTrans;
uniform float fogMidpoint;
uniform float fogSharpness;
uniform vec2 mouse;
uniform vec2 resolution;


float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
                 43758.5453123);
}



//------------------------------------------------------------
//------------------------------------------------------------
//	Simplex 3D Noise
//	by Ian McEwan, Ashima Arts
//------------------------------------------------------------
//------------------------------------------------------------
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);
    
    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;
    
    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );
    
    //  x0 = x0 - 0. + 0.0 * C
    vec3 x1 = x0 - i1 + 1.0 * C.xxx;
    vec3 x2 = x0 - i2 + 2.0 * C.xxx;
    vec3 x3 = x0 - 1. + 3.0 * C.xxx;
    
    // Permutations
    i = mod(i, 289.0 );
    vec4 p = permute( permute( permute(
                                       i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
                              + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
                     + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));
    
    // Gradients
    // ( N*N points uniformly over a square, mapped onto an octahedron.)
    float n_ = 1.0/7.0; // N=7
    vec3  ns = n_ * D.wyz - D.xzx;
    
    vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)
    
    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)
    
    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);
    
    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );
    
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));
    
    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;
    
    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);
    
    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    
    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                 dot(p2,x2), dot(p3,x3) ) );
}




void main(){
    //normalized frag coords
    vec2 uv = vec2(gl_FragCoord.x/resolution.x, gl_FragCoord.y/resolution.y);
    


    float speed = time * noiseSpeed;
    float sideDrift = time * driftSpeed;
    
    float bright = 0;
    
    //layer the noise smoke
    float numLayers = fogLayers;
    
    for(int i = 0; i < numLayers; i++){
        float offset = i * 2000.0f;
        float octave = i * octaveDiff;
        float x = gl_FragCoord.x * (noiseScale + octave) + sideDrift;
        float y = gl_FragCoord.y * (noiseScale + octave) * ySquish;

        bright += (1.0f/numLayers) * snoise( vec3(x, y, speed + offset) );
        
    }
    
    //invert (to get more smoke since this noise
    //generator has more black space than white)
    bright = 1.0f - bright;
    
    //The black space is solid black, so scale
    //it down to kill whiteout patches
    bright *= 0.8;
    
    
    //now give it an envelope to to only show smoke on the bottom half
    //manual piecewise exponential easeInOut:
    float nY = 1.0 - gl_FragCoord.y/resolution.y;
    float env = 0.0f;
    if( nY < fogMidpoint ){
        env = 0.5 * exp( fogSharpness * nY - fogSharpness * fogMidpoint);
    } else {
        env = -0.5 * exp( -fogSharpness * nY + fogSharpness * fogMidpoint) + 1.0f;
    }
    
    bright *= env;

    
    vec4 col = vec4( vec3(bright), bright * fogTrans);
    
    gl_FragColor = col;
    
    
}







