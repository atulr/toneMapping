uniform sampler2D sampler0;
uniform vec2 TexelSize;

void main(void)

{
    vec4 sceneColor = texture2D(sampler0, gl_TexCoord[0].st);
      
    mat3 RGB2XYZ;
    RGB2XYZ[0] = vec3(0.5141364, 0.3238786, 0.16036376);
    RGB2XYZ[1] = vec3(0.265068, 0.67023428, 0.06409157);
    RGB2XYZ[2] = vec3(0.0241188, 0.1228178, 0.84442666);
    
    
    vec3 XYZ = RGB2XYZ*sceneColor.rgb; 
    
    // XYZ -> Yxy conversion   
     
    vec3 Yxy; 
    Yxy.r = XYZ.g; 
    
//    x = X / (X + Y + Z) 
//    y = X / (X + Y + Z) 
    
    float temp = dot(vec3 (1.0,1.0,1.0), XYZ.rgb); 
    
    Yxy.gb = XYZ.rg / temp;
    
    const float DELTA = 0.0001;
    
    float fLogLumSum = 0.0;
    //TexelSize, I think, should be the dimension of the image.. e.g. 2048, 1024 so on..
    fLogLumSum += log(dot(texture2D(sampler0, gl_TexCoord[0].xy + TexelSize * vec2(-0.5, -0.5)).rgb, Yxy) + DELTA);
    fLogLumSum += log(dot(texture2D(sampler0, gl_TexCoord[0].xy + TexelSize * vec2(-0.5, 0.5)).rgb, Yxy) + DELTA);
    fLogLumSum += log(dot(texture2D(sampler0, gl_TexCoord[0].xy + TexelSize * vec2(0.5, 0.5)).rgb, Yxy) + DELTA);
    fLogLumSum += log(dot(texture2D(sampler0, gl_TexCoord[0].xy + TexelSize * vec2(0.5, -0.5)).rgb, Yxy) + DELTA);
    
    gl_FragData[0] = vec4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0);
//    gl_FragData[0] = vec4(1.0, 1.0, 0, 1.0);
//    gl_FragData[0] = vec4(fLogLumSum, fLogLumSum, fLogLumSum, 1.0);
//    gl_FragColor = vec4(1.0, 0, 0, 1.0);
    

}