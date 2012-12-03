#include "sampleGUI.h"
#include "hdrloader.h"
#include "rgbe.h"
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
int width = 767, height= 1023, width1, height1, components;
float luminance[1023][767];
float luminanceImage[1023][767][3];

float *hdrImage;
unsigned *imageTex;
HDRLoaderResult result;
rgbe_header_info info;
int a;
// where the drawing stuff should go
static float RGB2XYZ  [3][3] = {{0.5141364, 0.3238786,  0.16036376},
    {0.265068,  0.67023428, 0.06409157},
    {0.0241188, 0.1228178,  0.84442666}};

float logAverage(float *img) {
    float sum; 
    double lum;
    for (int i=0; i<width*height; i++)
        sum += log(.00001 + img[i]);
    lum = exp(sum/(float) (width * height));
    printf("lum %f\n", lum);
    return lum;
}

float get_maxvalue ()
{
    float max = 0.;
    
    for (int i = 0; i < width * height; i++)
        max = (max < hdrImage[i]) ? hdrImage[i] : max;
    return max;
}

void copyLuminance() {
    int x, y;
    float img[width*height];
    
    for (x = 0; x < height; x++)
        for (y = 0; y < width; y++) {
            
            luminance[y][x] = luminanceImage[x][y][0];
            img[width * x + y] = luminanceImage[x][y][0];
        }
    scaleImageToMidTone(img);
//    glEnable(GL_TEXTURE_2D);
//
//    GLuint lumText;
//    glGenTextures(1, &lumText);
//    glBindTexture(GL_TEXTURE_2D, lumText);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
////    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
//    glBindTexture(GL_TEXTURE_2D,0);
//   
//    glDisable(GL_TEXTURE_2D);
//    writeTextureToPPM("/Users/atulrungta/Desktop/textureSaved1", lumText, width, height, GL_RGBA, GL_UNSIGNED_BYTE);


}

void scaleImageToMidTone(float *img) {
    float alpha = 0.18f;
    float scaleFactor;
    scaleFactor = 1.0 / logAverage(img);
    hdrImage[0] *= scaleFactor * alpha;
    for(int i=0;i<width; i++)
        for(int j=0;j<height;j++) {
            int startAddressOfPixel = ((i*width) + j);
            int addr = ((i*width) + j+3);
            hdrImage[addr] *= scaleFactor * alpha;
            img[startAddressOfPixel] *= scaleFactor * alpha; 
        }
    
//    FILE *f;
//    f = fopen("/Users/atulrungta/Desktop/cathedralMapped.hdr","wb");
//    RGBE_WriteHeader(f,width,height,&info);
//    RGBE_WritePixels(f,hdrImage,width*height);
//    fclose(f);
//    glEnable(GL_TEXTURE_2D);
//       
//        glGenTextures(1, &lumText);
//        glBindTexture(GL_TEXTURE_2D, lumText);
//        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
////        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
////        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
//
//    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, getImage());
//        glEnable( GL_TEXTURE_2D );
//        glBindTexture(GL_TEXTURE_2D,lumText);
//        drawScene();
//        glDisable(GL_TEXTURE_2D);
    
//        writeTextureToPPM("/Users/atulrungta/Desktop/textureSaved1", lumText, width, height, GL_RGB, GL_UNSIGNED_BYTE);
    
}


//since log average luminance is not working with shaders.. trying to calculate without shaders in a function..
void calculateLogAverageLuminance() {

//    unsigned char *image = new unsigned char;
    
//    int width, height, components;
//    width = result.width;
//    height = result.height;
    
    //    for (int i = 0; i < 2; i++) {
//    glGenTextures(2, inOutTexture);

//    width = getWidth();
//    height = getHeight();
//    image = (unsigned char *) malloc(width * height * 4);
//    image = getImage();
//    components = getComponents();
    float red, blue, green, alpha;
    float rgb[3], XYZ[3];
    
    float W;
 
// x is the width, y is the height;
    
    for (int i=0; i< height; i++){
        for(int j = 0 ; j<width; j++) {
            int startAddressOfPixel = ((i*width) + j);
            red = hdrImage[startAddressOfPixel];
            blue = hdrImage[startAddressOfPixel+1];
            green = hdrImage[startAddressOfPixel+2];
//            printf("red %f\n", red);
//            printf("blue %f\n", blue);
//            printf("green %f\n", green);

            rgb[0] = (float)red;
            rgb[1] = (float)blue;
            rgb[2] = (float)green;

            XYZ[0] = RGB2XYZ[0][0] * red + RGB2XYZ[0][1] * green + RGB2XYZ[0][2] * blue;
            XYZ[1] = RGB2XYZ[1][0] * red + RGB2XYZ[1][1] * green + RGB2XYZ[1][2] * blue;
            XYZ[2] = RGB2XYZ[2][0] * red + RGB2XYZ[2][1] * green + RGB2XYZ[2][2] * blue;
            
//            for (int a = 0; a < 3; a++)
//                for (int b = 0; b < 3; b++)
            if ((W = XYZ[0] + XYZ[1] + XYZ[2]) > 0.0)
            {
                luminanceImage[i][j][0] = XYZ[1];         /* Y */
                luminanceImage[i][j][1] = XYZ[0] / W;     /* x */
                luminanceImage[i][j][2] = XYZ[1] / W;     /* y */
//                printf("li %f \n", luminanceImage[j][i][0]);
//                printf("strt %d \n", startAddressOfPixel);
            }
            else
                luminanceImage[i][j][0] = luminanceImage[i][j][1] = luminanceImage[i][j][2] = 0.;
        }
    }
    
//    
//    
}

void myGlutDisplay(	void )
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   // clears the screen
    
    glLoadIdentity(); // reset the modelview matrix to the default state
    gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);    // place the camera where we want

//    drawScene();
//    if(flag == 0) {
//    if (texCapture == 1)
//        captureSceneToTexture();
//    
////    ////    writeTextureToPPM("/Users/atulrungta/Desktop/textureSavedgreen", inOutTexture[1], 2048, 2048, GL_RGBA, GL_UNSIGNED_BYTE);
//    
//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   // clears the screen
//    
//    glLoadIdentity(); // reset the modelview matrix to the default state
//    gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);    // place the camera where we want
//    GLint reducedTextures, texelSize; 
//    texSizeX = 512;
//    texSizeY = 512;
//    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
//    glUseProgramObjectARB(logAverageProgram);
//
//    reducedTextures = glGetUniformLocationARB( logAverageProgram, "sampler0");
//
//    for(int i = 0;i < 4; i++) {
//        
//        setUpFrameBufferObj();
//        texelSize = glGetUniformLocationARB( logAverageProgram, "TexelSize");
//        
////        printf("%d", reducedTextures);
//        glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
//        glUniform1i(reducedTextures, 0);
//        glActiveTexture(GL_TEXTURE0);
////        
//        glBindTexture( GL_TEXTURE_2D, inOutTexture[readTex] );
//        glBegin(GL_QUADS);
//            glNormal3f(0.0, 0.0, -1.0);
//            glTexCoord2f(0.0,0.0); glVertex3f(-10,  0, 0);
//            glTexCoord2f(0.0,1.0); glVertex3f(-10, 10, 0);
//            glTexCoord2f(1.0,1.0); glVertex3f( 10, 10, 0);
//            glTexCoord2f(1.0,0.0); glVertex3f( 10,  0, 0);
//        glEnd();
////        glViewport(0, 0, texSizeX, texSizeY);
////        gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);    // place the camera where we want
//
//        flag = 1;
//        printf("flag %d", flag);
//
//        writeTextureToPPM("/Users/atulrungta/Desktop/textureSaved", inOutTexture[readTex], texSizeX, texSizeY, GL_RGBA, GL_UNSIGNED_BYTE);
//
//
//        texSizeX /= 2;
//        texSizeY /= 2;
//        
//        //ping-pong
//        writeTex = 1 - writeTex;
//        readTex = 1 - readTex;
//
//        glUniform2f(texelSize, texSizeX, texSizeY);
//    }
//    
//
//     glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);  
//    glDisable(GL_TEXTURE_2D);
//    glUseProgramObjectARB(0);
    glEnable(GL_TEXTURE_2D);
    
    glGenTextures(1, &lumText);
    glBindTexture(GL_TEXTURE_2D, lumText);
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, width, height, 0, GL_RGB, GL_FLOAT, hdrImage);
    glEnable( GL_TEXTURE_2D );
//    glBindTexture(GL_TEXTURE_2D,lumText);
    drawScene();
//    glDisable(GL_TEXTURE_2D);

        glFlush();
	glutSwapBuffers();
//    }
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
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, lumText);
    glBegin(GL_QUADS);


    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, -1.0);
    glTexCoord2f(0.0,0.0); glVertex3f(-10,  0, 0);
    glTexCoord2f(0.0,1.0); glVertex3f(-10, 10, 0);
    glTexCoord2f(1.0,1.0); glVertex3f( 10, 10, 0);
    glTexCoord2f(1.0,0.0); glVertex3f( 10,  0, 0);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    
    glPopAttrib();

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
//    glClearColor(1.0, 0, 0, 1);
//    glViewport(0,0, texSizeX,texSizeY);   // set viewport to texture size
    
    
//    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
    checkGLFrameBufferError(GL_FRAMEBUFFER, "After FBO binding");
    
//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);   // clears the screen
    
//    glLoadIdentity(); // reset the modelview matrix to the default state
//    gluLookAt(eye[0], eye[1], eye[2], lookat[0], lookat[1], lookat[2], 0, 1, 0);    // place the camera where we want
    
    // draws the scene
//    drawScene();
    
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    checkGLFrameBufferError(GL_FRAMEBUFFER, "After releasing buffer");
    
    //should be used only for debugging; writing texture to disk is slow!!!!!!!!!!
    //put in a path you want your texture to be in
    writeTextureToPPM("/Users/atulrungta/Desktop/textureSaved", inOutTexture[readTex], texSizeX, texSizeY, GL_RGBA, GL_UNSIGNED_BYTE);
    
    
    texCapture = 0;     // toggle screen capture off
    
    myGlutReshape(0,0); // force reset of the viewport
}

// settiug up a frambeuffer object for capture to texture
// directly taken from: http://www.songho.ca/opengl/gl_fbo.html
int setUpFrameBufferObj(){
    // create the texture to capture the texture to
    glGenTextures(2, inOutTexture);
    unsigned* image = new unsigned;
    int width, height, components;
//    for (int i = 0; i < 2; i++) {
    glEnable(GL_TEXTURE_2D);
        image = getImage();
        width = getWidth();
        height = getHeight();
        components = getComponents();
        glBindTexture(GL_TEXTURE_2D, inOutTexture[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
       
    
        glBindTexture(GL_TEXTURE_2D, inOutTexture[1]);
        
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
//    }
//    resetTexture(inOutTexture[1], 512, 512, 0, 255, 0, 255);
    // create a renderbuffer object to store depth info
//    GLuint rboId;
//    glGenRenderbuffers(1, &rboId);
//    glBindRenderbuffer(GL_RENDERBUFFER, rboId);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, texSizeX, texSizeY);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);
//    
//    // create a framebuffer object
//    glGenFramebuffers(1, &fboId);
//    glBindFramebuffer(GL_FRAMEBUFFER, fboId);
//    
//    // attach the texture to FBO color attachment point
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, inOutTexture[readTex], 0);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, inOutTexture[writeTex], 0);
//    
//    // attach the renderbuffer to depth attachment point
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
//    
//    // check FBO status
//    GLenum status = checkGLFrameBufferError(GL_FRAMEBUFFER, "After creating buffer");
//    if (status != GL_FRAMEBUFFER_COMPLETE)
//        return 1;
//    
//    // switch back to window-system-provided framebuffer
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    
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
    
	// camera transform
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    
	glutPostRedisplay();
    

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
	glutInitWindowSize(512, 512);
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
//    bool ret = HDRLoader::load("/Users/atulrungta/Desktop/toneMapping/images/image3.hdr", result); 
    f = fopen("/Users/atulrungta/Desktop/toneMapping/images/cathedral.hdr","rb");
    RGBE_ReadHeader(f,&width,&height,&info);
    hdrImage = (float *)malloc(sizeof(float)*3*width*height);
    RGBE_ReadPixels_RLE(f,hdrImage,width,height);
    fclose(f);
    char file[100];
    strcpy(file, "/Users/atulrungta/Desktop/toneMapping/globalOperator");
    
//    bindShaders(logAverageProgram, vertexShader, fragmentShader, file);
//    setUpFrameBufferObj();

	// initialize the camera
	eye[0] = 0; 	eye[1] = 4;     eye[2] = 10;
	lookat[0] = 0;	lookat[1] = 0;	lookat[2] = 0;
    calculateLogAverageLuminance();
    copyLuminance();
//    std::string textureNames[2] = { "textures/StLouisArch512.rgb",  "textures/lightmap.rgb" };
//    imageTex = read_texture(textureNames[0].c_str(), &width1, &height1, &components);
                   
	
	// give control over to glut
	glutMainLoop();

	return 0;
}
