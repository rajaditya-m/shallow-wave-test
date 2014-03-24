UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CXX = g++
	GDB = gdb
	LIBS = -lglui -lglut -lGLEW -lGL -lGLU /usr/local/lib/libSOIL.a 
	OPTFLAGS = -w -O3
	NOOPTFLAGS = -w
	DIR6 = /usr/local/include
endif
ifeq ($(UNAME_S),Darwin)
	CXX = clang++
	GDB = lldb
	LIBS = -framework GLUI -framework OpenGL -framework GLUT -lGLEW -framework CoreFoundation /opt/local/lib/libSOIL.a 
	OPTFLAGS = -w -stdlib=libc++ -O3	 
	NOOPTFLAGS = -w -stdlib=libc++
	DIR6 = /opt/local/include/SOIL
endif

DIR1 = includes

DIR4 = helpers

DIR5 = /usr/local/include/eigen3

INC = $(DIR1) $(DIR4) $(DIR5)

INC_PARAMS = $(foreach d, $(INC), -I$d)

SOURCES = main.cpp

OBJECT = opengl.o

all : clean build run

clean :
	rm -rf opengl.o 

build :
	$(CXX) $(INC_PARAMS) $(SOURCES) $(LIBS) $(OPTFLAGS) -o $(OBJECT) 

run :
	./opengl.o

build_dbg :
	$(CXX) $(INC_PARAMS) $(SOURCES) $(LIBS) $(NOOPTFLAGS) -o $(OBJECT)  -g

run_dbg :
	$(GDB) opengl.o

dbg : clean build_dbg run_dbg