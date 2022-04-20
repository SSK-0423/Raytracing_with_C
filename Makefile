cc = g++
CFLAGS = 
TARGET = raytracing.out

#  ソースコード
SRCS += mymath.cpp
SRCS = raytracing_lib.cpp 
SRCS += myPng.cpp 
SRCS += log.cpp 
SRCS += raytracing_shading.cpp

OBJS = $(SRCS:.cpp=.o)

LIBDIR = 

LIBS = -lpng

$(TARGET): $(OBJS) 
		$(CC) -o $@ $^ $(LIBDIR) $(LIBS)

$(OBJS): $(SRCS)
		$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)