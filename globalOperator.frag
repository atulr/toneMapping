uniform sampler2D sampler0;


void main(void)

{
    vec4 sample = texture2D(sampler0, gl_TexCoord[0].st);
//    const float3x3 RGB2XYZ = {0.5141364, 0.3238786, 0.16036376, 0.265068, 0.67023428, 0.06409157, 0.0241188, 0.1228178, 0.84442666};
//    
//    float3 XYZ = mul(RGB2XYZ, FullScreenImage.rgb); 
//    
//    // XYZ -> Yxy conversion 
//    
//    float3 Yxy; 
//    
//    Yxy.r = XYZ.g; 
//    
//    // x = X / (X + Y + Z) 
//    // y = X / (X + Y + Z) 
//    
//    float temp = dot(float3 1.0,1.0,1.0), XYZ.rgb); 
//    
//    Yxy.gb = XYZ.rg / temp;
    
}