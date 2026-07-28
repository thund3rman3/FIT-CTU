CXX      = g++
LD       = g++

# Tvoje požadované flagy včetně -O2 pro obrovské zrychlení Ray Traceru
CXXFLAGS = -std=c++20 -Wall -pedantic -O2 -g -Isrc -Iexternal

OUT      = raytracer

SRCS     = $(wildcard src/*.cpp src/*/*.cpp)
OBJS     = $(SRCS:.cpp=.o)
DEPS     = $(OBJS:.o=.d)

.PHONY: all run compile clean

all: compile

run: compile
	./$(OUT)

compile: $(OUT)

$(OUT): $(OBJS)
	$(LD) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c -o $@ $<

clean:
	rm -f $(OBJS) $(DEPS) $(OUT)

-include $(DEPS)