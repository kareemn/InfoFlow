COMPILERFLAGS = -Wall 
CC = g++ 


OS = $(shell uname -s)
PROC = $(shell uname -p)
EXEC_SUFFIX=$(OS)-$(PROC)


ifeq ("$(OS)", "Darwin")
	FRAMEWORK = -framework GLUT -framework OpenGL 
	LIBPATH= -L"/System/Library/Frameworks/OpenGL.framework/Libraries"
	OSDEF=-DDARWIN
    LIBS= -lGL -lGLU -lm -lobjc -lstdc++ 
else
	OSLIB=
	OSINC= 
	OSDEF=-DLINUX
    LIBS= -lGL -lglut -lm -lstdc++ 
endif

CFLAGS = $(COMPILERFLAGS) $(OSDEF)



OBJECTS = infoflow.o
All: infoflow 

infoflow:	$(OBJECTS) infoflow.h
		$(CC) $(FRAMEWORK) $(CFLAGS) -o $@ $(LIBPATH) $(OBJECTS) $(LIBS)

infoflow.o: 
				$(CC) $(CFLAGS)  -c infoflow.cpp


clean:
		rm -rf infoflow $(OBJECTS)