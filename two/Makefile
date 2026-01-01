# Makefile: robust vecmath include/source detection

CC       := g++
CFLAGS   := -g -DSOLN

# Always include system GL headers
INCFLAGS := -I /usr/include/GL

# Try to find vecmath headers in a few likely places (local project first)
ifeq ($(wildcard ./vecmath/include/vecmath.h),./vecmath/include/vecmath.h)
VEC_INC := -I ./vecmath/include
else ifeq ($(wildcard /mit/6.837/public/include/vecmath.h),/mit/6.837/public/include/vecmath.h)
VEC_INC := -I /mit/6.837/public/include
else ifeq ($(wildcard ~/vecmath/include/vecmath.h),$(HOME)/vecmath/include/vecmath.h)
VEC_INC := -I $(HOME)/vecmath/include
else
VEC_INC := 
endif

INCFLAGS += $(VEC_INC)

# Add vecmath sources if the folder exists
VEC_SRCS := $(wildcard vecmath/src/*.cpp)
# If vecmath lib exists in the old MIT path, enable linking
VEC_LIB_FLAG :=
ifneq ($(wildcard /mit/6.837/public/lib/libvecmath.*),)
VEC_LIB_FLAG := -L /mit/6.837/public/lib -lvecmath
endif

# Link flags: OpenGL/GLUT + FLTK
LINKFLAGS  := -lglut -lGL -lGLU
LINKFLAGS += -lfltk -lfltk_gl
LINKFLAGS += $(VEC_LIB_FLAG)

SRCS      := bitmap.cpp camera.cpp MatrixStack.cpp modelerapp.cpp modelerui.cpp ModelerView.cpp Joint.cpp SkeletalModel.cpp Mesh.cpp main.cpp
SRCS     := $(SRCS) $(VEC_SRCS)
OBJS      := $(SRCS:.cpp=.o)
PROG      := a2

.PHONY: all clean depend

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LINKFLAGS)

# compile rule: pass include flags when compiling
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCFLAGS) -c $< -o $@

depend:
	-makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	-rm -f $(OBJS) $(PROG)

