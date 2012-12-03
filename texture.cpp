#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

#include "texture.h"

std::string textureNames[2] = { "textures/StLouisArch512.rgb",  "textures/lightmap.rgb" };

GLuint textures[2];
static unsigned *image = new unsigned ;
static int width, height, components;

void LoadGLTextures() {
    
//    static unsigned *image[2];
//    static int width, height, components;

    
    // allocate space for texture
//	for (int i=0; i<2; i++)
//		image[i] = new unsigned;
    
//    glEnable(GL_TEXTURE_2D);
    
    // Create Texture
//	glGenTextures(1, textures);
    
	image = read_texture(textureNames[0].c_str(), &width, &height, &components);
//    printf("%d \n", width);
//    printf("%d \n", height);
//	glBindTexture(GL_TEXTURE_2D, textures[0]);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image[0]);
//    glBindTexture(GL_TEXTURE_2D,0);
//    printf("%d width\n", width);
//    printf("%d height\n", height);
    
//	image[1] = read_texture(textureNames[1].c_str(), &width, &height, &components);
//	glBindTexture(GL_TEXTURE_2D, textures[1]);
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexImage2D(GL_TEXTURE_2D, 0, components, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image[1]);
	
//    glDisable(GL_TEXTURE_2D);
    
    // free that part
//	for (int i=0; i<2; i++)
//		free (image[i]);
}

unsigned* getImage() {
    return image;
}

int getHeight(){
    return height;
}

int getWidth() {
    return width;
}

int getComponents() {
    return components;
}
//--- Anything below is the read routine of SGI's "RGB" image format --

void
bwtorgba(unsigned char *b,unsigned char *l,int n) {
    while(n--) {
        l[0] = *b;
        l[1] = *b;
        l[2] = *b;
        l[3] = 0xff;
        l += 4; b++;
    }
}

void
latorgba(unsigned char *b, unsigned char *a,unsigned char *l,int n) {
    while(n--) {
        l[0] = *b;
        l[1] = *b;
        l[2] = *b;
        l[3] = *a;
        l += 4; b++; a++;
    }
}

void
rgbtorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *l,int n) {
    while(n--) {
        l[0] = r[0];
        l[1] = g[0];
        l[2] = b[0];
        l[3] = 0xff;
        l += 4; r++; g++; b++;
    }
}

void
rgbatorgba(unsigned char *r,unsigned char *g,unsigned char *b,unsigned char *a,unsigned char *l,int n) {
    while(n--) {
        l[0] = r[0];
        l[1] = g[0];
        l[2] = b[0];
        l[3] = a[0];
        l += 4; r++; g++; b++; a++;
    }
}

typedef struct _ImageRec {
    unsigned short imagic;
    unsigned short type;
    unsigned short dim;
    unsigned short xsize, ysize, zsize;
    unsigned int min, max;
    unsigned int wasteBytes;
    char name[80];
    unsigned long colorMap;
    FILE *file;
    unsigned char *tmp, *tmpR, *tmpG, *tmpB;
    unsigned long rleEnd;
    unsigned int *rowStart;
    int *rowSize;
} ImageRec;

static void
ConvertShort(unsigned short *array, long length) {
    unsigned b1, b2;
    unsigned char *ptr;
    
    ptr = (unsigned char *)array;
    while (length--) {
        b1 = *ptr++;
        b2 = *ptr++;
        *array++ = (b1 << 8) | (b2);
    }
}

static void
ConvertLong(unsigned *array, long length) {
    unsigned b1, b2, b3, b4;
    unsigned char *ptr;
    
    ptr = (unsigned char *)array;
    while (length--) {
        b1 = *ptr++;
        b2 = *ptr++;
        b3 = *ptr++;
        b4 = *ptr++;
        *array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
    }
}

static ImageRec *ImageOpen(const char *fileName)
{
    union {
        int testWord;
        char testByte[4];
    } endianTest;
    ImageRec *image;
    int swapFlag;
    int x;
    
    endianTest.testWord = 1;
    if (endianTest.testByte[0] == 1) {
        swapFlag = 1;
    } else {
        swapFlag = 0;
    }
    
    image = (ImageRec *)malloc(sizeof(ImageRec));
    if (image == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    if ((image->file = fopen(fileName, "rb")) == NULL) {
        perror(fileName);
        exit(1);
    }
    
    fread(image, 1, 12, image->file);
    
    if (swapFlag) {
        ConvertShort(&image->imagic, 6);
    }
    
    image->tmp = (unsigned char *)malloc(image->xsize*256);
    image->tmpR = (unsigned char *)malloc(image->xsize*256);
    image->tmpG = (unsigned char *)malloc(image->xsize*256);
    image->tmpB = (unsigned char *)malloc(image->xsize*256);
    if (image->tmp == NULL || image->tmpR == NULL || image->tmpG == NULL ||
        image->tmpB == NULL) {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    
    if ((image->type & 0xFF00) == 0x0100) {
        x = image->ysize * image->zsize * sizeof(unsigned);
        image->rowStart = (unsigned *)malloc(x);
        image->rowSize = (int *)malloc(x);
        if (image->rowStart == NULL || image->rowSize == NULL) {
            fprintf(stderr, "Out of memory!\n");
            exit(1);
        }
        image->rleEnd = 512 + (2 * x);
        fseek(image->file, 512, SEEK_SET);
        fread(image->rowStart, 1, x, image->file);
        fread(image->rowSize, 1, x, image->file);
        if (swapFlag) {
            ConvertLong(image->rowStart, x/sizeof(unsigned));
            ConvertLong((unsigned *)image->rowSize, x/sizeof(int));
        }
    }
    return image;
}

static void
ImageClose(ImageRec *image) {
    fclose(image->file);
    free(image->tmp);
    free(image->tmpR);
    free(image->tmpG);
    free(image->tmpB);
    free(image);
}

static void
ImageGetRow(ImageRec *image, unsigned char *buf, int y, int z) {
    unsigned char *iPtr, *oPtr, pixel;
    int count;
    
    if ((image->type & 0xFF00) == 0x0100) {
        fseek(image->file, image->rowStart[y+z*image->ysize], SEEK_SET);
        fread(image->tmp, 1, (unsigned int)image->rowSize[y+z*image->ysize],
              image->file);
        
        iPtr = image->tmp;
        oPtr = buf;
        while (1) {
            pixel = *iPtr++;
            count = (int)(pixel & 0x7F);
            if (!count) {
                return;
            }
            if (pixel & 0x80) {
                while (count--) {
                    *oPtr++ = *iPtr++;
                }
            } else {
                pixel = *iPtr++;
                while (count--) {
                    *oPtr++ = pixel;
                }
            }
        }
    } else {
        fseek(image->file, 512+(y*image->xsize)+(z*image->xsize*image->ysize),
              SEEK_SET);
        fread(buf, 1, image->xsize, image->file);
    }
}

unsigned *
read_texture(const char *name, int *width, int *height, int *components) {
    unsigned char *base, *lptr;
    unsigned char *rbuf, *gbuf, *bbuf, *abuf;
    ImageRec *image;
    int y;
    
    image = ImageOpen(name);
    
    if(!image)
        return NULL;
    (*width)=image->xsize;
    (*height)=image->ysize;
    (*components)=image->zsize;
    base = (unsigned char*)malloc(image->xsize*image->ysize*sizeof(unsigned));
    rbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    gbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    bbuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    abuf = (unsigned char *)malloc(image->xsize*sizeof(unsigned char));
    if(!base || !rbuf || !gbuf || !bbuf)
        return NULL;
    lptr = base;
    for(y=0; y<image->ysize; y++) {
        if(image->zsize>=4) {
            ImageGetRow(image,rbuf,y,0);
            ImageGetRow(image,gbuf,y,1);
            ImageGetRow(image,bbuf,y,2);
            ImageGetRow(image,abuf,y,3);
            rgbatorgba(rbuf,gbuf,bbuf,abuf,(unsigned char *)lptr,image->xsize);
            lptr += image->xsize;
        } else if(image->zsize==3) {
            ImageGetRow(image,rbuf,y,0);
            ImageGetRow(image,gbuf,y,1);
            ImageGetRow(image,bbuf,y,2);
            rgbtorgba(rbuf,gbuf,bbuf,(unsigned char *)lptr,image->xsize);
            lptr += image->xsize;
        } else if(image->zsize==2) {
            ImageGetRow(image,rbuf,y,0);
            ImageGetRow(image,abuf,y,1);
            latorgba(rbuf,abuf,(unsigned char *)lptr,image->xsize);
            lptr += image->xsize;
        } else {
            ImageGetRow(image,rbuf,y,0);
            bwtorgba(rbuf,(unsigned char *)lptr,image->xsize);
            lptr += image->xsize;
        }
    }
    ImageClose(image);
    free(rbuf);
    free(gbuf);
    free(bbuf);
    free(abuf);
    
    return (unsigned *) base;
}
