
main: main_gpu.c
	gcc -g main_gpu.c -o main_gpu -lOpenCL
clean:
	rm -rf main_gpu
