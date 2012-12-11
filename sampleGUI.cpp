#include "sampleGUI.h"
#include "hdrloader.h"
//#include "rgbe.h"
#include <fftw3.h>
/////////////////////////////////////////////////////////////////
//// Main opengl stuff

GLuint vert,frag,prog;
GLenum attachmentpoints[] = { GL_COLOR_ATTACHMENT0_EXT,
    GL_COLOR_ATTACHMENT1_EXT
};
GLuint lumText;
struct Yxy{
    float r, g, b;
}yxy;

int CurrActiveBuffer = 0;
int readTex = CurrActiveBuffer;
int writeTex = 1-CurrActiveBuffer;
int flag = 0;
int width = 644, height= 874, width1, height1, components;
float *luminance;
float *luminanceImage;
float gammaval = 1.6;
float *hdrImage, *hdrImageCopy;
float *convolvedImage, convolvedImageAtScale[8][767][1023]; //remember to change width and height
unsigned *imageTex;
HDRLoaderResult result;
fftw_complex *out, *out1;
fftw_plan plan, plan1, plan2;
float lW, lD;
// where the drawing stuff should go
static float RGB2XYZ  [3][3] = {{0.5141364, 0.3238786,  0.16036376},
    {0.265068,  0.67023428, 0.06409157},
    {0.0241188, 0.1228178,  0.84442666}};
static float XYZ2RGB  [3][3] = {{ 2.5651,   -1.1665,   -0.3986},
    {-1.0217,    1.9777,    0.0439},
    { 0.0753,   -0.2543,    1.1892}};


float logAverage(float *img) {
    float sum = 0.f;; 
    double lum = 0.f;
    for (int i=0; i<width*height; i++) {
        sum += log(.00001 + luminance[i]);
    }
    lum = exp(sum/(float) (width * height));
    printf("sum %f", sum);
    lW = lum;
    printf("avg lum %f", lum);
    return lum;
}

float get_maxvalue ()
{
    float max = 0.;
    
    for (int i = 0; i < width * height; i++)
        max = (max < luminance[i]) ? luminance[i] : max;
    return max;
}

void copyLuminance() {
    int x, y;
    float img[width*height];
    img[0] = hdrImage[0];
    int c = 0;
//    for (x = 0; x < height * width; x++) {  
//            luminance[x] = hdrImage[c];
//        c+=3;
//        }
    scaleImageToMidTone(luminance);
    simpleOperator(luminance);
//    reinhardOperator();

}

void imageFFT() {
    plan = fftw_plan_dft_r2c_2d(width, height, (double*)luminance, out, FFTW_ESTIMATE);
    fftw_execute(plan);
    
    double fft_scale = 1.0/sqrt(width * height);
    for (int i=0; i<height * width; i++) {
        out[i][0] *= fft_scale;
        out[i][1] *= fft_scale;
    }

}

void gaussianFFT(int scale) {
    
    double *filter;
    filter = (double *)malloc(sizeof(double)*width*height);
    plan1 = fftw_plan_dft_r2c_2d(width, height, filter, out1, FFTW_ESTIMATE);
    double a1 = 0.35;
    for (int a=1; a<=scale; a++) {
        a1 *= a1 * 1.6;
    }
    double key;
    key = (a1 * scale) * (a1 * scale);
    for (int i = 0; i<height; i++)
        for(int j = 0; j<width; j++) {
            filter[i * width + j] = (1.0/(3.14159 * key)) * exp( - (i * i + j*j)/key);
        }
    double fft_scale = 1.0/sqrt(width * height);
    fftw_execute(plan1);
    for (int i=0; i<height * width; i++) {
        out1[i][0] *= fft_scale;
        out1[i][1] *= fft_scale;
    }

}

void convoleImage(int scale) {
    fftw_complex *convolution;

    convolvedImage = (float *)malloc(sizeof(float)*width*height);

    convolution = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)  * width * height);
    for (int i = 0 ; i< width * height; i++) {
        convolution[i][0] = out[i][0] * out1[i][0] -  out[i][1] * out1[i][1];
        convolution[i][1] = out[i][0] * out1[i][1] +  out[i][1] * out1[i][0];
    }
    plan2 = fftw_plan_dft_c2r_2d(width, height, convolution, (double*)convolvedImage, FFTW_ESTIMATE);
    fftw_execute(plan2);
}

void assignImage(int scale) {
    for (int i=0;i<height; i++)
        for(int j=0;j<width; j++) {
            convolvedImageAtScale[scale-1][j][i] = convolvedImage[i * width + j];
        }
}

void reinhardOperator() {
    int scaleToUse = 1;
    float gaussianLum, Ld;
    int c = 0, size = 0;
    for(int i = 0;i<height; i++)
        for(int j=0;j<width;j++) {
            for(int k = 1; k<=8; k++) {
                gaussianFFT(k);
                convoleImage(k);
                assignImage(k);
                if(fabs(activity(k, i, j)) > 0.05f) {
                    scaleToUse = k;
                    break;
                }
            }
            gaussianLum = V1(scaleToUse, i, j); 
            Ld = luminance[size]/(1.0 + gaussianLum);
            hdrImage[c] = powf((hdrImage[c]/lW), 0.7) * Ld;
            hdrImage[c+1] = powf((hdrImage[c+1]/lW), 0.7) * Ld;
            hdrImage[c+2] = powf((hdrImage[c+2]/lW), 0.7) * Ld;
           
            c+=3;size++;
        }
    clamp_image();
           FILE *f;
           f = fopen("/Users/atulrungta/Desktop/memorialReinhardOperator.hdr","wb");
           RGBE_WriteHeader(f,width,height,&info);
           RGBE_WritePixels(f,hdrImage,width*height);
           fclose(f);    


}

void simpleOperator(float *img) {
    float Ld, lMax;
    int j;
    int c = 0;

    lMax = get_maxvalue();
    printf("L %f \n", lW);
    for(int i = 0; i< width * height; i++) {
        Ld = luminance[i]/(1.0 + luminance[i]);
        hdrImage[c] = powf((hdrImage[c]/lW), 0.7) * Ld;
        hdrImage[c+1] = powf((hdrImage[c+1]/lW), 0.7) * Ld;
        hdrImage[c+2] = powf((hdrImage[c+2]/lW), 0.7) * Ld;
        c+=3;
        
    }
    //    for(int i = 0; i < width * height; i++) {
//        hdrImage[c] = hdrImage[c] * (1. + (hdrImage[c] / lMax)) / (1. + hdrImage[c]);
//        c+=3;
//
//    }
    clamp_image();
        
//        FILE *f;
//        f = fopen("/Users/atulrungta/Desktop/memorialSimpleOperator.hdr","wb");
//        RGBE_WriteHeader(f,width,height,&info);
//        RGBE_WritePixels(f,hdrImage,width*height);
//        fclose(f);    

        
}

float V1(int scale, int x, int y) {
    return convolvedImageAtScale[scale][x][y];
}

float activity(int scale, int x, int y) {
    float difference, V;
    difference = V1(scale, x, y) - V1(scale + 1, x, y);
    V = difference/((256.0/(float)(scale * scale)) + V1(scale, x, y));
    return V;
}

void clamp_image ()
{
    int x, y, c= 0;
    
    for (y = 0; y < width*height; y++) {
            hdrImage[c] = (hdrImage[c] > 1.) ? 1. : hdrImage[c];
            hdrImage[c + 1] = (hdrImage[c + 1] > 1.) ? 1. : hdrImage[c + 1];
            hdrImage[c + 2] = (hdrImage[c + 2] > 1.) ? 1. : hdrImage[c + 2];
            c+=3;
    }

    FILE *f;
    f = fopen("/Users/atulrungta/Desktop/cathedralToneMapped.hdr","wb");
    
    RGBE_WriteHeader(f,width,height,&info);
    RGBE_WritePixels(f,hdrImage,width*height);
    fclose(f);    

}

void YxyToRGB() {
    int       x, y, i, j, c=0;
    float    result[3];
    float    X, Y, Z;
    
    for (x = 0; x < width*height; x++){

            Y         = hdrImage[c];        /* Y */
            result[1] = hdrImage[c + 1];        /* x */
            result[2] = hdrImage[c + 2];        /* y */
            if ((Y > 0.) && (result[1] > 0.) && (result[2] > 0.))
            {
                X = (result[1] * Y) / result[2];
                Z = (X/result[1]) - X - Y;
            }
            else
                X = Z = 0.;
            hdrImage[c] = X;
            hdrImage[c + 1] = Y;
            hdrImage[c+ 2] = Z;
            result[0] = result[1] = result[2] = 0.;
                   result[0] = XYZ2RGB[0][0] * hdrImage[c] + RGB2XYZ[0][1] * hdrImage[c + 1] + RGB2XYZ[0][2] * hdrImage[c + 2];
                   result[1] = XYZ2RGB[1][0] * hdrImage[c] + RGB2XYZ[1][1] * hdrImage[c + 1] + RGB2XYZ[1][2] * hdrImage[c + 2];
                   result[2] = XYZ2RGB[2][0] * hdrImage[c] + RGB2XYZ[2][1] * hdrImage[c + 1] + RGB2XYZ[2][2] * hdrImage[c + 2];
         for (i = 0; i < 3; i++)
                hdrImage[c + i] = result[i];
        c += 3;
        }
    
    clamp_image();

    
}

void scaleImageToMidTone(float *img) {
    float alpha = 0.18f;
    float scaleFactor;
    scaleFactor = 1.0 / logAverage(luminance);
    int c = 0;
    for(int i=0;i<height; i++)
        for(int j=0;j<width;j++) {
            int startAddressOfPixel = (i*width + j);
//            hdrImage[c] *= scaleFactor * alpha;
            luminance[startAddressOfPixel] *= scaleFactor * alpha; 
            c+=3;
        }
    
    
    
//    FILE *f;
//    f = fopen("/Users/atulrungta/Desktop/memorialMidTone.hdr","wb");
//    RGBE_WriteHeader(f,width,height,&info);
//    RGBE_WritePixels(f,hdrImage,width*height);
//    fclose(f);    
}


//since log average luminance is not working with shaders.. trying to calculate without shaders in a function..
void calculateLogAverageLuminance() {

    float red, blue, green, alpha;
    float rgb[3], XYZ[3],rgb1[3];
    int c= 0;
    float W;
// x is the width, y is the height;

    for (int i=0; i< height; i++){
        for(int j = 0 ; j<width; j++) {
            int startAddressOfPixel = c;
            rgb[0] = hdrImage[startAddressOfPixel];
            rgb[1] = hdrImage[startAddressOfPixel+1];
            rgb[2] = hdrImage[startAddressOfPixel+2];
            luminance[i*width + j] = .27 * rgb[0] + .67 * rgb[1] + 0.06 * rgb[2];
            

//            XYZ[0] = RGB2XYZ[0][0] * rgb[0] + RGB2XYZ[0][1] * rgb[1] + RGB2XYZ[0][2] * rgb[2];
//            XYZ[1] = RGB2XYZ[1][0] * rgb[0] + RGB2XYZ[1][1] * rgb[1] + RGB2XYZ[1][2] * rgb[2];
//            XYZ[2] = RGB2XYZ[2][0] * rgb[0] + RGB2XYZ[2][1] * rgb[1] + RGB2XYZ[2][2] * rgb[2];
//            W = XYZ[0] + XYZ[1] + XYZ[2];
//            if (W > 0.0)
//            {   
//                hdrImage[startAddressOfPixel] = XYZ[1];         /* Y */
//                hdrImage[startAddressOfPixel + 1] = XYZ[0] / W;     /* x */
//                hdrImage[startAddressOfPixel + 2] = XYZ[1] / W;     /* y */
//
//            }
//            else
//                hdrImage[startAddressOfPixel] = hdrImage[startAddressOfPixel + 1] = hdrImage[startAddressOfPixel + 2] = 0.;
            c+=3;
        }
    }
//    FILE *f;
//    f = fopen("/Users/atulrungta/Desktop/memorialXYZ.hdr","wb");
//    RGBE_WriteHeader(f,width,height,&info);
//    RGBE_WritePixels(f,hdrImage,width*height);
//    fclose(f);    

//    
//    
}

void myGlutDisplay(	void )
{   glViewport(0,0, width,height);
//    flag = 1;
    if(flag == 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);
        checkGLFrameBufferError(GL_FRAMEBUFFER, "After FBO binding");
        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   // clears the screen

//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   // clears the screen
    
    glLoadIdentity(); // reset the modelview matrix to the default state
//    gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, -1, 0);    // place the camera where we want
    glUseProgramObjectARB(logAverageProgram);
    GLint lWLocation, lMaxLocation;
    glEnable(GL_TEXTURE_2D);
    lWLocation = glGetUniformLocationARB( logAverageProgram, "Lw");
    lMaxLocation = glGetUniformLocationARB( logAverageProgram, "lMax");
    glUniform1f(lWLocation, lW);
    glUniform1f(lMaxLocation, get_maxvalue());
        glGenTextures(1, &lumText);
    glBindTexture(GL_TEXTURE_2D, lumText);
//    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, width, height, 0, GL_RGB, GL_FLOAT, hdrImageCopy);
    glDisable(GL_TEXTURE_2D);
    glEnable( GL_TEXTURE_2D );
        flag = 1;
//    glBindTexture(GL_TEXTURE_2D,lumText);
//        resetTexture(lumText, width, height, 255, 255, 0, 255);
        glMatrixMode (GL_MODELVIEW);
        glPushMatrix ();
        glLoadIdentity ();
        glMatrixMode (GL_PROJECTION);
        glPushMatrix ();
        glLoadIdentity ();
        

        drawScene();

//    glDisable(GL_TEXTURE_2D);
    glUseProgramObjectARB(0);
        
        glLoadIdentity(); // reset the modelview matrix to the default state
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        checkGLFrameBufferError(GL_FRAMEBUFFER, "After releasing buffer");


//        captureSceneToTexture();

        flag = 1;
        
        glEnd ();
        glPopMatrix ();
        glMatrixMode (GL_MODELVIEW);
        glPopMatrix ();
        writeTextureToPPM("/Users/atulrungta/Desktop/textureSaved", textureCapture, width, height, GL_RGBA, GL_UNSIGNED_BYTE);


        glFlush();
//	glutSwapBuffers();
    }
}

void resetTexture(GLuint texName, int textureWidth, int textureHeight, int color_red, int color_green, int color_blue, int color_alpha){
    // cout << "resetTexture" << endl;
    unsigned char *textureArray;
    textureArray = new unsigned char[4*textureWidth*textureHeight];
    for (int i=0; i<(textureWidth*textureHeight); i++){
        textureArray[i*4 +0] = color_red;
        textureArray[i*4 +1] = color_green;
        textureArray[i*4 +2] = color_blue;
        textureArray[i*4 +3] = color_alpha;
    }
    
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureArray);
    
    delete[] textureArray;
    textureArray = NULL;
}
// drawing elements of the scene
void drawScene(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, lumText);
          glBegin (GL_QUADS);
          glNormal3f(0.0, 0.0, 1.0);
          glTexCoord2f(0.0,0.0); glVertex3f(-1, -1, -1);
          glTexCoord2f(1.0,0.0); glVertex3f(1, -1, -1);
          glTexCoord2f(1.0,1.0); glVertex3f( 1, 1, -1);
          glTexCoord2f(0.0,1.0); glVertex3f( -1, 1, -1);
    glEnd();

//    glBegin(GL_QUADS);
//    glNormal3f(0.0, 0.0, 1.0);
//    glTexCoord2f(0.0,0.0); glVertex3f(-10,  0, 0);
//    glTexCoord2f(0.0,1.0); glVertex3f(-10, 10, 0);
//    glTexCoord2f(1.0,1.0); glVertex3f( 10, 10, 0);
//    glTexCoord2f(1.0,0.0); glVertex3f( 10,  0, 0);
//    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    

}


// init openGL states
void initGL(){
    // display some opengl info
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "Shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;
    
    // load the textures you need
    LoadGLTextures();
    
    // sets up a frame buffer object
//    if (setUpFrameBufferObj() == 1 )
//        std::cout << "Framebuffer creation failed!!!" << std::endl << std::endl;

    // color to clear the screen to
    glClearColor(0, 0, 0, 0);
    
    // set some states
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
}

   
void captureSceneToTexture(){
    glViewport(0,0, width,height);   // set viewport to texture size
    
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    checkGLFrameBufferError(GL_FRAMEBUFFER, "After FBO binding");
    
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   // clears the screen
    
    glLoadIdentity(); // reset the modelview matrix to the default state
//    gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);    // place the camera where we want
    
    // draws the scene
    drawScene();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkGLFrameBufferError(GL_FRAMEBUFFER, "After releasing buffer");
    
    //should be used only for debugging; writing texture to disk is slow!!!!!!!!!!
    //put in a path you want your texture to be in
    writeTextureToPPM("/Users/atulrungta/Desktop/textureSaved", textureCapture, width, height, GL_RGBA, GL_UNSIGNED_BYTE);
    
    
    texCapture = 0;     // toggle screen capture off
    
    myGlutReshape(0,0); // force reset of the viewport
}

// settiug up a frambeuffer object for capture to texture
// directly taken from: http://www.songho.ca/opengl/gl_fbo.html
int setUpFrameBufferObj(){
    glGenTextures(1, &textureCapture);
    glBindTexture(GL_TEXTURE_2D, textureCapture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    // create a renderbuffer object to store depth info
    GLuint rboId;
    glGenRenderbuffers(1, &rboId);
    glBindRenderbuffer(GL_RENDERBUFFER, rboId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
    // create a framebuffer object
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    
    // attach the texture to FBO color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureCapture, 0);
    
    // attach the renderbuffer to depth attachment point
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
    
    // check FBO status
    GLenum status = checkGLFrameBufferError(GL_FRAMEBUFFER, "After creating buffer");
    if (status != GL_FRAMEBUFFER_COMPLETE)
        return 1;
    
    // switch back to window-system-provided framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return 0;
}


/////////////////////////////////////////////////////////////////
///// Input stuff

// catch mouse up/down events
void myGlutMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
		cur_button = button;
	else
	{
		if (button == cur_button)
			cur_button = -1;
	}

	last_x = x;
	last_y = y;
}


// catch mouse move events
void myGlutMotion(int x, int y)
{
	// the change in mouse position
	int dx = x-last_x;
	int dy = y-last_y;

	float scale, len, theta;
	float neye[3], neye2[3];
	float f[3], r[3], u[3];

	switch(cur_button)
	{
	case GLUT_LEFT_BUTTON:
		// translate
		f[0] = lookat[0] - eye[0];
		f[1] = lookat[1] - eye[1];
		f[2] = lookat[2] - eye[2];
		u[0] = 0;
		u[1] = 1;
		u[2] = 0;

		// scale the change by how far away we are
		scale = sqrt(length(f)) * 0.007;

		crossproduct(f, u, r);
		crossproduct(r, f, u);
		normalize(r);
		normalize(u);

		eye[0] += -r[0]*dx*scale + u[0]*dy*scale;
		eye[1] += -r[1]*dx*scale + u[1]*dy*scale;
		eye[2] += -r[2]*dx*scale + u[2]*dy*scale;

		lookat[0] += -r[0]*dx*scale + u[0]*dy*scale;
		lookat[1] += -r[1]*dx*scale + u[1]*dy*scale;
		lookat[2] += -r[2]*dx*scale + u[2]*dy*scale;

		break;

	case GLUT_MIDDLE_BUTTON:
		// zoom
		f[0] = lookat[0] - eye[0];
		f[1] = lookat[1] - eye[1];
		f[2] = lookat[2] - eye[2];

		len = length(f);
		normalize(f);

		// scale the change by how far away we are
		len -= sqrt(len)*dx*0.03;

		eye[0] = lookat[0] - len*f[0];
		eye[1] = lookat[1] - len*f[1];
		eye[2] = lookat[2] - len*f[2];

		// make sure the eye and lookat points are sufficiently far away
		// push the lookat point forward if it is too close
		if (len < 1)
		{
			printf("lookat move: %f\n", len);
			lookat[0] = eye[0] + f[0];
			lookat[1] = eye[1] + f[1];
			lookat[2] = eye[2] + f[2];
		}

		break;

	case GLUT_RIGHT_BUTTON:
		// rotate

		neye[0] = eye[0] - lookat[0];
		neye[1] = eye[1] - lookat[1];
		neye[2] = eye[2] - lookat[2];

		// first rotate in the x/z plane
		theta = -dx * 0.007;
		neye2[0] = (float)cos(theta)*neye[0] + (float)sin(theta)*neye[2];
		neye2[1] = neye[1];
		neye2[2] =-(float)sin(theta)*neye[0] + (float)cos(theta)*neye[2];


		// now rotate vertically
		theta = -dy * 0.007;

		f[0] = -neye2[0];
		f[1] = -neye2[1];
		f[2] = -neye2[2];
		u[0] = 0;
		u[1] = 1;
		u[2] = 0;
		crossproduct(f, u, r);
		crossproduct(r, f, u);
		len = length(f);
		normalize(f);
		normalize(u);

		neye[0] = len * ((float)cos(theta)*f[0] + (float)sin(theta)*u[0]);
		neye[1] = len * ((float)cos(theta)*f[1] + (float)sin(theta)*u[1]);
		neye[2] = len * ((float)cos(theta)*f[2] + (float)sin(theta)*u[2]);

		eye[0] = lookat[0] - neye[0];
		eye[1] = lookat[1] - neye[1];
		eye[2] = lookat[2] - neye[2];

		break;
	}


	last_x = x;
	last_y = y;

	glutPostRedisplay();
}

// you can put keyboard shortcuts in here
void myGlutKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	// quit
	case 27: 
	case 'q':
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}



/////////////////////////////////////////////////////////////////
///// some opengl utilities stuff

// the window has changed shapes, fix ourselves up
void myGlutReshape(int	x, int y)
{
	int tx, ty, tw, th;
    //	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
    //	glViewport(tx, ty, tw, th);
    //    glViewport(0, 0, 2048, 2048);    
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1, 1, -1, 1, 1, 1000);
//    gluOrtho2D(0, width, 0, height);
	// camera transform
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    
	glutPostRedisplay();
}


// make sure the main window is active
void myGlutIdle(void)
{
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);
	
	// just keep redrawing the scene over and over
	glutPostRedisplay();
}
char* readFile(const char* filename) {
 	FILE *fp;
	char *content = NULL;
    
	int count=0;
	if (filename != NULL) {
		fp = fopen(filename,"rt");
        
		if (fp != NULL) {
            
            fseek(fp, 0, SEEK_END);
            count = ftell(fp);
            rewind(fp);
            
			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
    
}

void bindShaders(GLhandleARB & glsl_program, GLhandleARB & vertexShader, GLhandleARB & fragmentShader, char fileName[]) {
	const GLcharARB* vertex_shader_source;
	const GLcharARB* fragment_shader_source;
	GLint status;
    char fName[100];
    strcpy(fName, fileName);

	// Create the OpenGL shader objects
	glsl_program    = glCreateProgramObjectARB();
	vertexShader   = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	
	// Load the source code
	vertex_shader_source = readFile(strcat(fileName, ".vert"));
	fragment_shader_source = readFile(strcat(fName, ".frag"));
	
	glShaderSourceARB(vertexShader, 1, &vertex_shader_source, NULL);
//    printf("%s", 
	glShaderSourceARB(fragmentShader, 1, &fragment_shader_source, NULL);
	
	delete vertex_shader_source;
	delete fragment_shader_source;
	
	
	// Compile the shaders
	glCompileShaderARB(vertexShader);
	glGetObjectParameterivARB(vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
	if( GL_FALSE == status ) {
		// Get the InfoLog and print the compilation error
		GLint msglen;
		glGetObjectParameterivARB(vertexShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &msglen);
		GLcharARB* msg = (GLcharARB*)malloc(msglen);
		glGetInfoLogARB(vertexShader, msglen, NULL, msg);
        std::cout << "Error compiling vertex shader : " << msg << std::endl;
		free(msg);
		exit(EXIT_FAILURE);
	}
	
	glCompileShaderARB(fragmentShader);
	glGetObjectParameterivARB(fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
	if( GL_FALSE == status ) {
		// Get the InfoLog and print the compilation error
		GLint msglen;
		glGetObjectParameterivARB(fragmentShader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &msglen);
		GLcharARB* msg = (GLcharARB*)malloc(msglen);
		glGetInfoLogARB(fragmentShader, msglen, NULL, msg);
        std::cout << "Error compiling fragment shader : " << msg << std::endl;
		free(msg);
		exit(EXIT_FAILURE);
	}
	
	// Attach the compiled shader to the program object
	glAttachObjectARB(glsl_program, vertexShader);
	glAttachObjectARB(glsl_program, fragmentShader);
	
	// Link the program
	glLinkProgramARB(glsl_program);
	glGetObjectParameterivARB(glsl_program, GL_OBJECT_LINK_STATUS_ARB, &status);
	if( GL_FALSE == status ) {
		// Get the InfoLog and print the linking error
		GLint msglen;
		glGetObjectParameterivARB(glsl_program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &msglen);
		GLcharARB* msg = (GLcharARB*)malloc(msglen);
		glGetInfoLogARB(glsl_program, 1024, NULL, msg);
        std::cout << "Error linking GLSL program : " << msg << std::endl;
		free(msg);
		exit(EXIT_FAILURE);
	}
    
}


/////////////////////////////////////////////////////////////////
///// UI stuff

// some controls generate a callback when they are changed
void glui_cb(int control)
{

	switch(control)
	{
	case CB_DEPTH_BUFFER:
		if (live_use_depth_buffer)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
		break;
	case CB_ACTION_BUTTON:
		if (step < 0)
		{
			step = 0;
			action_button->set_name("Stop");
		}
		else
		{
			step = -1;
			action_button->set_name("Start");
		}
		break;
            
	case CB_RESET:
		//
		// put your reset callback here
		//
		break;
	}

	glutPostRedisplay();
}

void glui_button(int control){
	switch(control){
		case TEX_CAPTURE:
			texCapture = 1-texCapture;
			break;
            
        default:
            break;
    }
}


// the UI code
void initGLUI(){
    //
	// create the interface subwindow and add widgets
	//
//	glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_LEFT);
    glui = GLUI_Master.create_glui("GLUI");
	// initialize live variables
	live_object_type = 0;
	live_object_xz_trans[0] = 0;
	live_object_xz_trans[1] = 0;
	live_object_y_trans = 0;
	live_draw_floor = 1;
	live_draw_object = 1;
	live_use_depth_buffer = 1;
    
	// quit button
	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);
    
	// empty space
	glui->add_statictext("");
    
	// the object rollout
	object_rollout = glui->add_rollout("Object");
    
	// the radio buttons
	object_type_radio = glui->add_radiogroup_to_panel(object_rollout, &live_object_type);
	glui->add_radiobutton_to_group(object_type_radio, "cube");
	glui->add_radiobutton_to_group(object_type_radio, "sphere");
	glui->add_radiobutton_to_group(object_type_radio, "torus");
    
	// rotation and translation controls
	// we need an extra panel to keep things grouped properly
	GLUI_Panel *transform_panel = glui->add_panel_to_panel(object_rollout, "", GLUI_PANEL_NONE);
	object_rotation = glui->add_rotation_to_panel(transform_panel, "Rotation", live_object_rotation);
	object_rotation->reset();
    
	glui->add_column_to_panel(transform_panel, false);
	object_xz_trans = glui->add_translation_to_panel(transform_panel, "Translate XZ",
													 GLUI_TRANSLATION_XY, live_object_xz_trans);
    
	glui->add_column_to_panel(transform_panel, false);
	object_y_trans =  glui->add_translation_to_panel(transform_panel, "Translate Y",
													 GLUI_TRANSLATION_Y, &live_object_y_trans);
    
	object_xz_trans->scale_factor = 0.1f;
	object_y_trans->scale_factor = 0.1f;
    
	glui->add_button_to_panel(object_rollout, "Reset Object Transform", CB_RESET, glui_cb);
    
	// empty space
	glui->add_statictext("");
    
	// the walk control
	anim_rollout = glui->add_rollout("Animation");
    
	action_button = glui->add_button_to_panel(anim_rollout, "Stop", CB_ACTION_BUTTON, glui_cb);
    
	GLUI_Spinner *spin_s =
    
    glui->add_spinner_to_panel(anim_rollout, "Speed", GLUI_SPINNER_FLOAT, &live_anim_speed);
    
	spin_s->set_float_limits(0.1, 10.0);
    
	// our checkbox options for deciding what to draw
	glui->add_checkbox("Draw Floor", &live_draw_floor);
	glui->add_checkbox("Draw Object", &live_draw_object);
    
	// empty space
	glui->add_statictext("");
    
	glui->add_checkbox("Use Depth Buffer", &live_use_depth_buffer, CB_DEPTH_BUFFER, glui_cb);
    
    
    // empty space
	glui->add_statictext("");
    
    glui->add_button("Capture to texture", TEX_CAPTURE, glui_button);
    
	glui->set_main_gfx_window(main_window);
}


/////////////////////////////////////////////////////////////////
///// entry point

int main(int argc,	char* argv[])
{
    glutInit(&argc, argv);
    
	//
	// create the glut window
	//
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100,100);
    
    

	main_window = glutCreateWindow("Sample Interface");
	
	//
	// set callbacks
	//
	glutDisplayFunc(myGlutDisplay);
	GLUI_Master.set_glutReshapeFunc(myGlutReshape);
	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	GLUI_Master.set_glutKeyboardFunc(myGlutKeyboard);
	GLUI_Master.set_glutMouseFunc(myGlutMouse);
	glutMotionFunc(myGlutMotion);
     
    // initialize gl
    initGL();
    
    // init the interface
	initGLUI();
    FILE *f;
    f = fopen("/Users/atulrungta/Desktop/toneMapping/images/desk.hdr","rb");
    RGBE_ReadHeader(f,&width,&height,&info);
    hdrImage = (float *)malloc(sizeof(float)*3*width*height);

    RGBE_ReadPixels_RLE(f,hdrImage,width,height);
    fclose(f);
    
    f = fopen("/Users/atulrungta/Desktop/toneMapping/images/desk.hdr","rb");
    RGBE_ReadHeader(f,&width,&height,&info);
    hdrImageCopy = (float *)malloc(sizeof(float)*3*width*height);
//    
    RGBE_ReadPixels_RLE(f,hdrImageCopy,width,height);
    fclose(f);
    
    
    char file[100];
    strcpy(file, "/Users/atulrungta/Desktop/toneMapping/globalOperator");
    luminance = (float *)malloc(sizeof(float)*width*height);
//    convolvedImageAtScale = (float *)malloc(sizeof(float)*width*height*8);

//    bool ret = HDRLoader::load("/Users/atulrungta/Desktop/toneMapping/images/cathedral.hdr", result); 
    bindShaders(logAverageProgram, vertexShader, fragmentShader, file);
    setUpFrameBufferObj();

	// initialize the camera
	eye[0] = 0; 	eye[1] = 4;     eye[2] = 10;
	lookat[0] = 0;	lookat[1] = 0;	lookat[2] = 0;
    
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)  * width * height);
    out1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)  * width * height);


    calculateLogAverageLuminance();
    copyLuminance();
//    imageFFT();

//    YxyToRGB();
//    std::string textureNames[2] = { "textures/StLouisArch512.rgb",  "textures/lightmap.rgb" };
//    imageTex = read_texture(textureNames[0].c_str(), &width1, &height1, &components);
                   
//    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * width * height);
    fftw_destroy_plan(plan);   
    fftw_destroy_plan(plan1);   
    fftw_destroy_plan(plan2);   
    
    	// give control over to glut
	glutMainLoop();

	return 0;
}
