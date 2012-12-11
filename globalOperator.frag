uniform sampler2D sampler0;

//const float step_w = 1.0/767.0;
//const float step_h = 1.0/1023.0;

uniform float Lw;
uniform float lMax;

float width = 644.0;
float height = 874.0;

void convolution(in mat3 R, out float sum) {
    float step_w = 1.0/width;
    float step_h = 1.0/height;
    float i, j;
    int c = 0,a, b;
    vec2 offset[9];
    vec4 color;
    mat3 lumVals;
    float lum;
    
    for(i = -1.0; i<1.0; i++)
        for(j=-1.0; j<1.0; j++) {
            offset[c] = vec2(j*step_w, i*step_h);
            c = c + 1;
        }
    c = 0;
    for(a = 0; a< 3; a++)
        for(b = 0; b< 3; b++) {
            color = texture2D(sampler0, gl_TexCoord[0].st + offset[c]);
            lum = 0.27 * color.r + 0.67 * color.g + .06 * color.b;
            lumVals[a][b] = lum;
            c = c + 1;
        }
    sum = 0.0;
    for(a = 0; a < 3; a++)
        for(b = 0; b < 3; b++) {
            sum += lumVals[a][b] * R[a][b];
        }
}

void gaussian_s8(in float lum, in float x, in float y, in float alpha, out mat3 R) {
    float j, k;
    for(j = 0.0; j<3.0; j++) {
        for (k=0.0;k<3.0; k++) {
            R[int(j)][int(k)] = (1.0/(3.14159 * (alpha * 8.0) * (alpha * 8.0))) * exp(- ((x + k/width) * (x + k/width) + (y + j/height) * (y+j/height))/((alpha * 8.0) * (alpha * 8.0)));
            
        }
    }
}

void gaussian_s7(in float lum, in float x, in float y, in float alpha, out mat3 R) {
    float j, k;
    for(j = 0.0; j<3.0; j++) {
        for (k=0.0;k<3.0; k++) {
            R[int(j)][int(k)] = (1.0/(3.14159 * (alpha * 7.0) * (alpha * 7.0))) * exp(- ((x + k/width) * (x + k/width) + (y + j/height) * (y+j/height))/((alpha * 7.0) * (alpha * 7.0)));
            
        }
    }
    //    convolution(R);
    
}

void gaussian_s6(in float lum, in float x, in float y, in float alpha, out mat3 R) {
    float j, k;
    for(j = 0.0; j<3.0; j++) {
        for (k=0.0;k<3.0; k++) {
            R[int(j)][int(k)] = (1.0/(3.14159 * (alpha * 6.0) * (alpha * 6.0))) * exp(- ((x + k/width) * (x + k/width) + (y + j/height) * (y+j/height))/((alpha * 6.0) * (alpha * 6.0)));
            
        }
    }
    //    convolution(R);
    
}

void gaussian_s5(in float lum, in float x, in float y, in float alpha, out mat3 R) {
    float j, k;
    for(j = 0.0; j<3.0; j++) {
        for (k=0.0;k<3.0; k++) {
            R[int(j)][int(k)] = (1.0/(3.14159 * (alpha * 5.0) * (alpha * 5.0))) * exp(- ((x + k/width) * (x + k/width) + (y + j/height) * (y+j/height))/((alpha * 5.0) * (alpha * 5.0)));
            
        }
    }
    //    convolution(R);
    
}

void gaussian_s4(in float lum, in float x, in float y, in float alpha, out mat3 R) {
    float j, k;
    for(j = 0.0; j<3.0; j++) {
        for (k=0.0;k<3.0; k++) {
            R[int(j)][int(k)] = (1.0/(3.14159 * (alpha * 4.0) * (alpha * 4.0))) * exp(- ((x + k/width) * (x + k/width) + (y + j/height) * (y+j/height))/((alpha * 4.0) * (alpha * 4.0)));
            
        }
    }
    //    convolution(R);
    
}

void gaussian_s3(in float lum, in float x, in float y, in float alpha, out mat3 R) {
    float j, k;
    for(j = 0.0; j<3.0; j++) {
        for (k=0.0;k<3.0; k++) {
            R[int(j)][int(k)] = (1.0/(3.14159 * (alpha * 3.0) * (alpha * 3.0))) * exp(- ((x + k/width) * (x + k/width) + (y + j/height) * (y+j/height))/((alpha * 3.0) * (alpha * 3.0)));
            
        }
    }
    //    convolution(R);
    
}


void gaussian_s2(in float lum, in float x, in float y, in float alpha, out mat3 R) {
    float j, k;
    for(j = 0.0; j<3.0; j++) {
        for (k=0.0;k<3.0; k++) {
            R[int(j)][int(k)] = (1.0/(3.14159 * (alpha * 2.0) * (alpha * 2.0))) * exp(- ((x + k/width) * (x + k/width) + (y + j/height) * (y+j/height))/((alpha * 2.0) * (alpha * 2.0)));
            
        }
    }
    //    convolution(R);
    
}
void gaussian_s1(in float lum, in float x, in float y, in float alpha, out mat3 R) {
    float j, k;
    for(j = 0.0; j<3.0; j++) {
        for (k=0.0;k<3.0; k++) {
            R[int(j)][int(k)] = (1.0/(3.14159 * (alpha * 1.0) * (alpha * 1.0))) * exp(- ((x + k/width) * (x + k/width) + (y + j/height) * (y+j/height))/((alpha * 1.0) * (alpha * 1.0)));
            
        }
    }
    //        convolution(R);
}

void Activity(in float lum, in float x, in float y,  in int scale, out float val) {
    float alpha = 0.35;
    mat3 R1, R2;
    float s1, s2;
    if(scale == 1) { 
        gaussian_s1(lum, x, y, alpha, R1);
        convolution(R1, s1);
        gaussian_s1(lum, x, y, alpha * 1.6, R2);
        convolution(R2, s2);
    }
    if(scale == 2) {
        gaussian_s2(lum, x, y, alpha, R1);
        convolution(R1, s1);
        gaussian_s2(lum, x, y, alpha * 1.6, R2);
        convolution(R2, s2);

    }
    if(scale == 3) {
        gaussian_s3(lum, x, y, alpha, R1);
        convolution(R1, s1);
        gaussian_s3(lum, x, y, alpha * 1.6, R2);
        convolution(R2, s2);
    }
    if(scale == 4) {
        gaussian_s4(lum, x, y, alpha, R1);
        convolution(R1, s1);
        gaussian_s4(lum, x, y, alpha * 1.6, R2);
        convolution(R2, s2);
    }
    if(scale == 5) {
        gaussian_s5(lum, x, y, alpha, R1);
        convolution(R1, s1);
        gaussian_s5(lum, x, y, alpha * 1.6, R2);
        convolution(R2, s2);

    }
    if(scale == 6) {
        gaussian_s6(lum, x, y, alpha, R1);
        convolution(R1, s1);
        gaussian_s6(lum, x, y, alpha * 1.6, R2);
        convolution(R2, s2);

    }
    if(scale == 7) {
        gaussian_s7(lum, x, y, alpha, R1);
        convolution(R1, s1);
        gaussian_s7(lum, x, y, alpha * 1.6, R2);
        convolution(R2, s2);

    }
    if(scale == 8) { 
        gaussian_s8(lum, x, y, alpha, R1);
        convolution(R1, s1);

        gaussian_s8(lum, x, y, alpha * 1.6, R2);
        convolution(R2, s2);

    }
    
    val = (s1 - s2)/((pow(2.0, 8.0) * 1.8)/ pow(float(scale), 2.0) + s1);

}


void main(void)

{
    vec4 sceneColor = texture2D(sampler0, gl_TexCoord[0].st);
      
    const mat3 RGB2XYZ = mat3(0.5141364, 0.3238786,  0.16036376,
        0.265068,  0.67023428, 0.06409157,
        0.0241188, 0.1228178,  0.84442666);				                    
    vec3 XYZ = RGB2XYZ * sceneColor.rgb;
////    
    vec3 Yxy;
    Yxy.r = XYZ.g;                            // copy luminance Y
    Yxy.g = XYZ.r / (XYZ.r + XYZ.g + XYZ.b ); // x = X / (X + Y + Z)
    Yxy.b = XYZ.g / (XYZ.r + XYZ.g + XYZ.b ); // y = Y / (X + Y + Z)
    float x = gl_TexCoord[0].x;
    float y = gl_TexCoord[0].y;
    float lum = Yxy.r;
    float threshold = 0.01;
    float Lp = lum * 0.18 / Lw;  
    
//    
    const mat3 XYZ2RGB  = mat3( 2.5651,-1.1665,-0.3986,
        -1.0217, 1.9777, 0.0439, 
        0.0753, -0.2543, 1.1892);
    int i;
    float val, s;
    mat3 M;
    for (i = 0; i < 8; i++) {
        Activity(lum, x, y, i, val);
        if(abs(val) < threshold) {
            if(i == 1) {
                gaussian_s1(lum, x, y, .35, M); 
                convolution(M, s);
            }
            if(i == 2) {
                gaussian_s2(lum, x, y, .35, M);  
                convolution(M, s);
                
            }
            if(i == 3) {
                gaussian_s3(lum, x, y, .35, M);  
                convolution(M, s);
                
            }
            if(i == 4) {
                gaussian_s4(lum, x, y, .35, M);  
                convolution(M, s);
                
            }
            if(i == 5) {
                gaussian_s5(lum, x, y, .35, M);  
                convolution(M, s);
                
            }
            if(i == 6) {
                gaussian_s6(lum, x, y, .35, M);  
                convolution(M, s);
                
            }
            if(i == 7) {                
                gaussian_s7(lum, x, y, .35, M);  
                convolution(M, s);

            }
            if(i == 8) {
                gaussian_s8(lum, x, y, .35, M);  
                convolution(M, s);

            }
            break;
        }
    }
    
//    float Ld = (Lp * (1.0 + Lp/(lMax * lMax)))/(1.0 + Lp); 
    float Ld = Lp/ ( 1.0 + s);

//    vec4 color;
     
    float X, Y, Z;
    vec3 result;

    Y = Ld;
    result[1] = Yxy.g;
    result[2] = Yxy.b;
    if ((Y > 0.0) && (result[1] > 0.0) && (result[2] > 0.0))
    {
        X = (result[1] * Y) / result[2];
        Z = (X/result[1]) - X - Y;
    }
    else
        X = Z = 0.;
    Yxy.r = X;
    Yxy.g = Y;
    Yxy.b = Z;
    
    result[0] = result[1] = result[2] = 0.0;
    
    result = XYZ2RGB * Yxy;
    
    
//    XYZ.r = Yxy.r * Yxy.g / Yxy. b;               // X = Y * x / y
//    XYZ.g = Ld;                                // copy luminance Y
//    XYZ.b = Yxy.r * (1.0 - Yxy.g - Yxy.b) / Yxy.b;  // Z = Y * (1-x-y) / y

    vec3 color = XYZ2RGB * XYZ;
    vec4 fooColor;
//    float gamma = 1.6;
    fooColor.r = result.r;
    fooColor.g = result.g;
    fooColor.b = result.b;
    
    gl_FragData[0] = vec4(fooColor.r, fooColor.g, fooColor.b, 1.0);
//    gl_FragData[0] = vec4(0, 1.0, 0,1.0);

}