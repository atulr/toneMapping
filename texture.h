#ifndef TEXTURE
#define TEXTURE

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include "GL/glut.h"
#endif

extern GLuint textures[2];


// load the textures
void LoadGLTextures();


/*
 * read_texture() - read in an image file in SGI 'libimage' format
 * 	currently its very simple minded and converts all images
 *      to RGBA8 regardless of the input format and returns the
 *	original number of components in the appropriate parameter.
 *
 *
 *	the components are converted as follows
 *		L    -> LLL 1.0
 *		LA   -> LLL A
 *		RGB  -> RGB 1.0
 *		RGBA -> RGB A
 *
 */
unsigned *  read_texture(const char *name, int *width, int *height, int *components);

#endif
