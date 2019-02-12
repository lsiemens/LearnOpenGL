Path=#Path ...
GLAD=-I$(Path)/include
FLAGS=-ldl
TARGET=camera_hello

crun:clean $(TARGET).out
	$(bash) ./$(TARGET).out

run:$(TARGET).out
	$(bash) ./$(TARGET).out

glad.o:glad.c
	gcc -c glad.c $(GLAD) -o glad.o

template_window.out:glad.o
	g++ template_window.cpp glad.o $(FLAGS) $(GLAD) -lglfw -o template_window.out

triangle_hello.out:glad.o
	g++ triangle_hello.cpp glad.o $(FLAGS) $(GLAD) -lglfw -o triangle_hello.out

shaders_hello.out:glad.o
	g++ shaders_hello.cpp glad.o $(FLAGS) $(GLAD) -lglfw -o shaders_hello.out

textures_hello.out:glad.o
	g++ textures_hello.cpp stb_image.cpp glad.o $(FLAGS) $(GLAD) -lglfw -o textures_hello.out

transforms_hello.out:glad.o
	g++ transforms_hello.cpp stb_image.cpp glad.o $(FLAGS) $(GLAD) -lglfw -o transforms_hello.out

coordinate_hello.out:glad.o
	g++ coordinate_hello.cpp stb_image.cpp glad.o $(FLAGS) $(GLAD) -lglfw -o coordinate_hello.out

camera_hello.out:glad.o
	g++ camera_hello.cpp stb_image.cpp glad.o $(FLAGS) $(GLAD) -lglfw -o camera_hello.out

.PHONY:clean
clean:
	rm -f *.o *.out
