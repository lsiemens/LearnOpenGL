ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

INCLUDE=$(ROOT_DIR)/include
CFLAGS=-g -I$(INCLUDE)
LINKFLAGS=-ldl -lglfw
TARGET=breakout

breakout.out:glad.o stb_image.o shader.o texture.o resource_manager.o sprite_renderer.o game.o
	g++ breakout.cpp glad.o stb_image.o shader.o texture.o resource_manager.o sprite_renderer.o game.o $(LINKFLAGS) $(CFLAGS) -o breakout.out

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
	g++ -c shader.cpp $(LINKFLAGS) $(CFLAGS) -o shader.o

texture.o:
	g++ -c texture.cpp $(LINKFLAGS) $(CFLAGS) -o texture.o

resource_manager.o:
	g++ -c resource_manager.cpp $(LINKFLAGS) $(CFLAGS) -o resource_manager.o

sprite_renderer.o:
	g++ -c sprite_renderer.cpp $(LINKFLAGS) $(CFLAGS) -o sprite_renderer.o

game.o:
	g++ -c game.cpp $(LINKFLAGS) $(CFLAGS) -o game.o

.PHONY:clean
clean:
	rm -f *.o *.out

.PHONY:pclean
pclean:
	rm -f game.o *.out

