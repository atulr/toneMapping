#ifndef getting_started_sampleGLUI_h
#define getting_started_sampleGLUI_h

#include "texture.h"
#include "Color.h"

#include "GL/glui.h"

#include <math.h>
#include <iostream>
#include <string>
#include <jpeglib.h>
#include <stdio.h>
#include <stdlib.h>
#include "rgbe.h"
rgbe_header_info info;

//////////////////////////////////////////////////////////////////////////
// Some constants
// the user id's that we can use to identify which control
// caused the callback to be called
#define CB_DEPTH_BUFFER 0
#define CB_ACTION_BUTTON 1
#define CB_RESET 2
#define TEX_CAPTURE 3



//////////////////////////////////////////////////////////////////////////
// Global variables

// main window
int	main_window;

// the camera info
float eye[3];
float lookat[3];

// more textures
GLuint fboId;
GLuint textureCapture;
GLuint inOutTexture[2];
int texSizeX = 512;
int texSizeY = 512;
int texCapture = 0;


// mouse handling functions for the main window
// left mouse translates, middle zooms, right rotates
// keep track of which button is down and where the last position was
int cur_button = -1;
int last_x;
int last_y;


// pointers for all of the glui controls
GLUI *glui;
GLUI_Rollout		*object_rollout;
GLUI_RadioGroup		*object_type_radio;
GLUI_Rotation		*object_rotation;
GLUI_Translation	*object_xz_trans;
GLUI_Translation	*object_y_trans;

GLUI_Rollout		*anim_rollout;
GLUI_Button			*action_button;

GLUI_Checkbox *draw_floor_check;
GLUI_Checkbox *draw_object_check;

// This  checkbox utilizes the callback
GLUI_Checkbox *use_depth_buffer;


// walking action variables
//
GLfloat step = 0;
GLfloat live_anim_speed = 3;


// live variables
// each of these are associated with a control in the interface.
// when the control is modified, these variables are automatically updated
int live_object_type;	// 0=cube, 1=sphere, 2=torus
float live_object_rotation[16];
float live_object_xz_trans[2];
float live_object_y_trans;
int live_draw_floor;
int live_draw_object;
int live_use_depth_buffer;
int live_texture_capture;
GLhandleARB logAverageProgram;
GLhandleARB vertexShader;
GLhandleARB fragmentShader;

//////////////////////////////////////////////////////////////////////////
// Functions

// OpenGL stuff
void initGL();              // initialize some states
void drawScene();           // draws the scene

int setUpFrameBufferObj();  // sets up a FBO
void captureSceneToTexture();
void bindShaders(GLhandleARB & glsl_program, GLhandleARB & vertexShader, GLhandleARB & fragmentShader, char fileName[]);
char* readFile(const char* filename);
// opengl callbacks that need to the registered
void myGlutDisplay(	void );                 // drawing the scene goes here
void myGlutIdle(void);                      // reswitching focus

void myGlutMouse(int button, int state, int x, int y);  // catch mouse up/down events
void myGlutKeyboard(unsigned char key, int x, int y);   // you can put keyboard shortcuts in here

void myGlutReshape(int	x, int y);                      // the window has changed shapes, fix ourselves up


// UI
void initGLUI();                // init the UI code
void glui_cb(int control);      // some controls generate a callback when they are changed
void calculateLogAverageLuminance();
void resetTexture(GLuint texName, int textureWidth, int textureHeight, int color_red, int color_green, int color_blue, int color_alpha);
float logAverage(float *img);
void scaleImageToMidTone(float *img);
void copyLuminance();
void simpleOperator(float *img);
float maxVal(float *img);
void YxyToRGB();
void clamp_image();

//////////////////////////////////////////////////////////////////////////
// Inline defined functions

// Some maths stuff
inline void normalize(float v[3]){
	float l = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	l = 1 / (float)sqrt(l);
    
	v[0] *= l;  v[1] *= l;  v[2] *= l;
}

inline void crossproduct(float a[3], float b[3], float res[3]){
	res[0] = (a[1] * b[2] - a[2] * b[1]);
	res[1] = (a[2] * b[0] - a[0] * b[2]);
	res[2] = (a[0] * b[1] - a[1] * b[0]);
}

inline float length(float v[3]){
	return (float)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}



// useful opengl debug
inline GLenum checkGLFrameBufferError(GLenum target, std::string message ="", bool warmNoError = false){
    GLenum err = glCheckFramebufferStatus(target);
    
    switch (err){
        case GL_FRAMEBUFFER_UNDEFINED: std::cout << "At " << message  << ":  " << "GL_FRAMEBUFFER_UNDEFINED: The target is the default framebuffer, but the default framebuffer does not exist" << std::endl;
            break;
            
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: std::cout << "At " << message  << ":  " <<  "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: Some/All of the framebuffer attachment points are framebuffer incomplete" << std::endl; break;
            
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cout << "At " << message  << ":  " << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: The framebuffer does not have at least one image attached to it" << std::endl;
            break;
            
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: std::cout << "At " << message  << ":  " << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for any color attachment point(s) named by GL_DRAWBUFFERi." << std::endl;
            break;
            
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: std::cout << "At " << message  << ":  " << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: The GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER" << std::endl;
            break;
            
        case GL_FRAMEBUFFER_UNSUPPORTED: std::cout << "At " << message  << ":  " <<"GL_FRAMEBUFFER_UNSUPPORTED: The combination of internal formats of the attached images violates an implementation-dependent set of restrictions" << std::endl;
            break;
            
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            std::cout << "At " << message  << ":  " << "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; if the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES" << std::endl;
            std::cout << "At " << message  << ":  " << "or the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, if the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures" << std::endl;
            break;
            
            // case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: 	std::cout << "The framebuffer attachment is layered, and any populated attachment is not layered, or if all populated color attachments are not from textures of the same target" << std::endl; break;
            
        case GL_INVALID_ENUM:
            std::cout << "At " << message  << ":  " << "generated if target is not GL_DRAW_FRAMEBUFFER, GL_READ_FRAMEBUFFER or GL_FRAMEBUFFER." << std::endl;
            break;
            
        default:
            if (warmNoError == true)
                std::cout << "No error - you are all good!" << std::endl;
    }
    
    return err;
}

inline GLenum checkGLError(std::string message ="", bool warmNoError = false){
    GLenum err = glGetError();
    
    switch (err){
        case GL_INVALID_ENUM: std::cout << "At " << message  << ":  "<< "GL_INVALID_ENUM: Given when an enumeration parameter contains an enum that is not allowed for that function" << std::endl; break;
            
        case GL_INVALID_VALUE: std::cout << "At " << message  << ":  "<< "GL_INVALID_VALUE: Given when a numerical parameter does not conform to the range requirements that the function places upon it" << std::endl; break;
            
        case GL_INVALID_OPERATION: std::cout << "At " << message  << ":  "<< "GL_INVALID_OPERATION: Given when the function in question cannot be executed because of state that has been set in the context" << std::endl; break;
            
        case GL_OUT_OF_MEMORY: std::cout << "At " << message  << ":  "<< "GL_OUT_OF_MEMORY: Given when performing an operation that can allocate memory, when the memory in question cannot be allocated" << std::endl; break;
            
        case GL_STACK_OVERFLOW: std::cout << "At " << message  << ":  "<< " GL_STACK_OVERFLOW: Deprecated - Given when a stack pushing operation causes a stack to overflow the limit of that stack's size." << std::endl; break;
            
        case GL_STACK_UNDERFLOW: std::cout << "At " << message  << ":  "<< "GL_STACK_UNDERFLOW: Deprecated - Given when a stack popping operation is given when the stack is already at its lowest point." << std::endl; break;
            
        case GL_TABLE_TOO_LARGE: std::cout << "At " << message  << ":  "<< "GL_TABLE_TOO_LARGE: This error code is a part of the GL_ARB_imaging extention " << std::endl; break;
            
        case GL_INVALID_FRAMEBUFFER_OPERATION: std::cout << "At " << message  << ":  "<< "GL_INVALID_FRAMEBUFFER_OPERATION: Check for framebuffer erros instead; these things have their own code!!!" << std::endl;
            break;
            
        case GL_NO_ERROR:
            if (warmNoError == true)
                std::cout << "At " << message << "  No error - you are all good!" << std::endl;
            break;
            
        default:
            std::cout << "At " << message << "  Something else which is not getting caught with code: " << err << std::endl;
            
    }
    
    return err;
}

//-----------------------------------------------------------------------------
// Name: writeTextureToPPM
// Desc: writes a ppm image
// Source: from Liang
// Example use: writeTextureToPPM("output/final", textureBuffer[readTex], textureDim,textureDim, GL_RGBA, GL_UNSIGNED_BYTE);
//-----------------------------------------------------------------------------
inline void writeTextureToPPM( const char* fileName , GLuint tex, GLuint m_iSizeX, GLuint m_iSizeY, GLenum m_format, GLenum m_type)
{
    //cout << "m_iSizeX: " <<m_iSizeX << "  m_iSizeY: " << m_iSizeY << endl;
    GLuint m_iSizePerElement = sizeof(GL_UNSIGNED_BYTE);
    unsigned char*content = new unsigned char[m_iSizeX * m_iSizeY * m_iSizePerElement];
    float *foo = new float[3 * m_iSizeX * m_iSizeY * sizeof(float)];
//    printf("%d x", m_iSizeX); 
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexImage(GL_TEXTURE_2D, 0, m_format, m_type, content);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    Image img(m_iSizeX, m_iSizeY);
    // Image img(m_iSizeXX, m_iSizeYY);
    
    int c = 0;
    float *hdrOutImage;
    hdrOutImage = (float *)malloc(sizeof(float)*3*m_iSizeY*m_iSizeX);

    for(GLuint y = 0; y < m_iSizeY; y++){
        for(GLuint x = 0; x < m_iSizeX; x++){
            Color C;
            size_t index = y * m_iSizeX + x;
            double alpha = double(content[4 * index + 3]) / 255.0;
//            alpha = 1.0;
            C.set_r(double(content[4 * index + 0]) / 255.0 * alpha);
            C.set_g(double(content[4 * index + 1]) / 255.0 * alpha);
            C.set_b(double(content[4 * index + 2]) / 255.0 * alpha);
            hdrOutImage[c] = double(content[4 * index + 0]) /255.0 ;
            hdrOutImage[c+1] = double(content[4 * index + 1]) /255.0;
            hdrOutImage[c+2] = double(content[4 * index + 2]) /255.0;
            c+=3;
            img.set(x, y, C);
        }
    }
          FILE *f;
          f = fopen("/Users/atulrungta/Desktop/memorialSimpleOperator.hdr","wb");
          RGBE_WriteHeader(f,m_iSizeX,m_iSizeY,&info);
          RGBE_WritePixels(f,hdrOutImage,m_iSizeX*m_iSizeY);
          fclose(f);    

    img.write(std::string(fileName) + ".ppm");
    
    delete [] content;
	content = NULL;
}

#endif
