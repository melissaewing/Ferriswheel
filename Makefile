FLAGS = -O2 -I/usr/include/GL
LIBS = -lglui -lglut -lGLU -lGL -lX11 -lm

all: ferriswheel 

ferriswheel: ferriswheel.cpp
	g++ ferriswheel.cpp -o hw3 $(FLAGS) $(LIBS)
