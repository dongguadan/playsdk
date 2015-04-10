CXX = g++
INC_DIR = ../include
CFLAGS += -I../3party/ffmpeg_linux/include/
CFLAGS += -I../3party/sdl/include/SDL2/
CFLAGS += -c -W -O2
CFLAGS += -fno-stack-protector
CFLAGS += -pipe -fomit-frame-pointer -fpic
CFLAGS += -I$(INC_DIR)
CXXFLAGS = $(CFLAGS)

LFLAGS = -L../dgdplaysdk -ldgdplay
LFLAGS += -L../3party/sdl/lib -lSDL2 -Wl,-rpath=../3party/sdl/lib
LFLAGS += -L../3party/ffmpeg_linux/lib -lavformat -lavdevice -lavcodec -lavutil -lswscale -lswresample
LFLAGS += -lpthread
LFLAGS += -lrt
LFLAGS += -ldl

HEADS = $(wildcard $(INC_DIR)/*.h)
SOURCES = $(wildcard ./*.cpp)
OBJS = $(SOURCES:.cpp=.o)
TARGET = ./sdktest2

all:$(TARGET)
$(TARGET):$(OBJS) $(SOURCES) $(HEADS)
	$(CXX) -o $@ $(OBJS) $(LFLAGS)

$(OBJS):%.o:%.cpp $(HEADS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) ./$(TARGET)