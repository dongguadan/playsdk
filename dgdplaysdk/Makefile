CXX = g++
INC_DIR = ../include
CFLAGS += -I$(INC_DIR)
CFLAGS += -I../3party/ffmpeg_linux/include/
CFLAGS += -I../3party/sdl/include/SDL2/
CFLAGS += -c -W -O2
CFLAGS += -fno-stack-protector
CFLAGS += -pipe -fomit-frame-pointer -fpic
CXXFLAGS = $(CFLAGS)


LFLAGS += -L../3party/ffmpeg_linux/lib -lavformat -lavdevice -lavcodec -lavutil -lswscale -lswresample
LFLAGS += -L../3party/sdl/lib -lSDL2
LFLAGS += -lpthread
LFLAGS += -lrt
LFLAGS += -ldl

HEADS = $(wildcard $(INC_DIR)/*.h)
SOURCES = $(wildcard ./*.cpp)
OBJS = $(SOURCES:.cpp=.o)
TARGET = ./libdgdplay.a
CXX = g++
AR  = ar rc

all:$(TARGET)

$(TARGET):$(OBJS) $(SOURCES) $(HEADS)
	$(AR) $@ $(OBJS)
$(OBJS):%.o:%.cpp $(HEADS)
	$(CXX) -c $< $(CXXFLAGS)

clean:
	rm -f $(OBJS) $(TARGET)
