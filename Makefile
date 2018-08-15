#TEST = $(shell g++ -std=c++17 src/main.cpp -MM)
SRC = $(shell find . -name "*.cpp")

# Recursively find all .cpp files in directory.
#SRC_FILES = $(shell find . -name "*.cpp" -printf "%f\n")

# Turn .cpp into .o and change path of .o files to /debug/
OBJS = $(patsubst ./src/%.cpp, ./debug/%.o, $(SRC))

# Set VPATH so all files can be accessed without full path.
VPATH = $(wildcard ./src/*):$(wildcard ./*)

#$(info $$var is [${OBJS}])echo Hello world 

CC = sudo g++
DEBUG = -g
CXXFLAGS = -std=c++17 -Wall -pedantic -MMD -MP -c $(DEBUG)
LFLAGS = -std=c++17 -Wall -pedantic $(DEBUG)

all: build run

build: dirs torus

dirs:
	@mkdir -p "debug"
	@mkdir -p $(dir $(OBJS))

torus: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o debug/torus

debug/%.o : %.cpp
	$(CC) $(CXXFLAGS) $< -o $@

debug/%.d: ;
.PRECIOUS: debug/%.d

run:
	./debug/torus

clean:
	rm -r debug/

tar:
	tar cfv torus.tar src/ 

include $(wildcard $(OBJS:%.o=%.d))
