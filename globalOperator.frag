uniform sampler2D sampler0;

uniform float Lw;

void main(void)

{
    vec4 sceneColor = texture2D(sampler0, gl_TexCoord[0].st);
      
    const mat3 RGB2XYZ = mat3(0.5141364, 0.3238786,  0.16036376,
        0.265068,  0.67023428, 0.06409157,
        0.0241188, 0.1228178,  0.84442666);				                    
    vec3 XYZ = RGB2XYZ * sceneColor.rgb;
    
    vec3 Yxy;
    Yxy.r = XYZ.g;                            // copy luminance Y
    Yxy.g = XYZ.r / (XYZ.r + XYZ.g + XYZ.b ); // x = X / (X + Y + Z)
    Yxy.b = XYZ.g / (XYZ.r + XYZ.g + XYZ.b ); // y = Y / (X + Y + Z)
    
    float Lp = Yxy.r * 0.18 / Lw;  
    
    XYZ.r = Yxy.r * Yxy.g / Yxy. b;               // X = Y * x / y
    XYZ.g = Yxy.r;                                // copy luminance Y
    XYZ.b = Yxy.r * (1.0 - Yxy.g - Yxy.b) / Yxy.b;  // Z = Y * (1-x-y) / y
    
    const mat3 XYZ2RGB  = mat3( 2.5651,-1.1665,-0.3986,
        -1.0217, 1.9777, 0.0439, 
        0.0753, -0.2543, 1.1892);
    vec4 color;
    color.rgb = XYZ2RGB * XYZ;
  //  color.a = 0.0;


    gl_FragData[0] = color;
    //gl_FragColor = vec4(1, 0,0,1.0);

}