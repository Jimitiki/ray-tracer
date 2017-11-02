CC = gcc

all: raytracer

raytracer: raytracer.c
	$(CC) -o raytracer $^ -lm

clean:
	rm -f raytracer
