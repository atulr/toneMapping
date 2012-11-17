#Created by Aaron Lefohn
#This makefile is designed to be used on the cs Linux workstations
#Modified by John Schreiner
#Modified by Pascal Grosset

SOURCE = sampleGUI.cpp texture.cpp
OBJECTS = $(SOURCE:.cpp=.o)
EXE = sample
CC = g++
CFLAGS = -c -Wall -DGL_GLEXT_PROTOTYPES

GLUI_INCLUDE=.

# To check for mac and linux
UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	GLUT_LIB=/usr/lib
	GLUT_INCLUDE=/usr/include
	GLUI_LIB=lib

	FLAGS = -I$(GLUI_INCLUDE) -I$(GLUT_INCLUDE) -L$(GLUT_LIB)  -L$(GLUI_LIB) \
	-lglui -lglut -lXmu  -lXext -lX11 -lGL -lGLU -lm -lGLEW

else
	
	FLAGS = -I$(GLUI_INCLUDE) -framework GLUT -framework GLUI -framework OpenGL
endif


all: $(SOURCE) $(EXE)

$(EXE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(FLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


#all: $(EXE)
#$(EXE): $(SOURCE)
#	$(CC) $(CFLAGS) -o $(EXE) $(SOURCE) $(FLAGS)


clean:
	rm -rf *.o $(EXE)
	clear