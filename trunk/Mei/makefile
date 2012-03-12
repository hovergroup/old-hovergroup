LIBDIR := ~/moos-ivp/lib
INCDIR := ~/moos-ivp/ivp/src/
TARGET := bin/listener

CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(patsubst src/%.cpp,obj/%.o,$(CPP_FILES))
LD_FLAGS := -lboost_system -lboost_thread
CC_FLAGS := -Wall
	
${TARGET}: $(OBJ_FILES)
	g++ ${CC_FLAGS} ${OBJ_FILES} -L ${LIBDIR} -o ${TARGET} ${LD_FLAGS} 
	
obj/%.o: src/%.cpp
	g++ ${CC_FLAGS} -I ${INCDIR} -c -o $@ $<

clean:
	rm -f ${TARGET} ${OBJ_FILES} *~
