LIBDIR := ~/moos-ivp/lib
INCDIR := ~/moos-ivp/ivp/src/
TARGET := bin/myapp

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp,obj/%.o,$(CPP_FILES))
LD_FLAGS := -lMOOS -lMOOSGen -lm -lpthread -lmbutil
CC_FLAGS := -Wall
	
${TARGET}: $(OBJ_FILES)
	g++ ${CC_FLAGS} ${OBJ_FILES} -L ${LIBDIR} -o ${TARGET} ${LD_FLAGS} 
	
obj/%.o: src/%.cpp
	g++ ${CC_FLAGS} -I ${INCDIR} -c -o $@ $<

clean:
	rm -f ${TARGET} ${OBJ_FILES} *~