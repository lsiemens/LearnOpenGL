ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

INCLUDE=$(ROOT_DIR)/include
CFLAGS=-g -I$(INCLUDE)
LINKFLAGS=-ldl -lglfw
TARGET=breakout
OBJECTS=glad.o stb_image.o shader.o texture.o resource_manager.o sprite_renderer.o \
        particle_generator.o game_object.o ball_object.o game_level.o game.o

breakout.out:$(OBJECTS)
	g++ breakout.cpp $(OBJECTS) $(LINKFLAGS) $(CFLAGS) -o breakout.out

prun:pclean $(TARGET).out
	$(bash) ./$(TARGET).out

crun:clean $(TARGET).out
	$(bash) ./$(TARGET).out

run:$(TARGET).out
	$(bash) ./$(TARGET).out

glad.o:
	gcc -c $(INCLUDE)/glad/glad.c $(CFLAGS) -o glad.o

stb_image.o:
	g++ -c $(INCLUDE)/stb_image/stb_image.cpp -o stb_image.o

shader.o:
	g++ -c shader.cpp $(CFLAGS) -o shader.o

texture.o:
	g++ -c texture.cpp $(CFLAGS) -o texture.o

resource_manager.o:
	g++ -c resource_manager.cpp $(CFLAGS) -o resource_manager.o

sprite_renderer.o:
	g++ -c sprite_renderer.cpp $(CFLAGS) -o sprite_renderer.o

particle_generator.o:
	g++ -c particle_generator.cpp $(CFLAGS) -o particle_generator.o

game_object.o:
	g++ -c game_object.cpp $(CFLAGS) -o game_object.o

ball_object.o:
	g++ -c ball_object.cpp $(CFLAGS) -o ball_object.o

game_level.o:
	g++ -c game_level.cpp $(CFLAGS) -o game_level.o

game.o:
	g++ -c game.cpp $(CFLAGS) -o game.o

.PHONY:clean
clean:
	rm -f *.o *.out

.PHONY:pclean
pclean:
	rm -f game.o *.out

