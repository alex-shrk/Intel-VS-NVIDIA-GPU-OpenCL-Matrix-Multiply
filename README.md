##### Intel VS NVIDIA GPU OpenCL Matrix Multiply


###### RUS:

Небольшой проект для сравнения производительности вычислений на интегрированном видеопроцессоре Intel, центральном процессоре Intel и видеокарте Nvidia.
Реализовано умножение матриц SGEMM самого базового типа с использованием OpenCL.


В коде под GPU реализован перебор всех платформ и выполнение расчетов на первом устройстве каждой платформы.
Время работы программы замеряется. Число запусков можно изменять. Первый запуск не идет в зачет и необходим для "холодного старта".



**Сборка и запуск:**

Для запуска вычислений на интегрированном видеопроцессоре используется реализация OpenCL для Intel GPU - _Beignet_ 
(_https://www.freedesktop.org/wiki/Software/Beignet/_).
Собираем, устанавливаем по инструкции из ссылки выше.

Для запуска вычислений на видеопроцессоре NVIDIA используется _CUDA Toolkit_ 

- Install the cuda toolkit with:

`sudo apt-get install nvidia-cuda-toolkit`

- Также нужно установить 'clinfo' для проверки работы OpenCL платформ и устройств

`sudo apt-get install clinfo`


- Собираем бинарник для CPU:

`cd ./cpu`

`sudo make`

- Запуск под CPU

`./main_cpu`

Можно сделать вывод в лог файл командой

`./main_cpu > cpu.log`

- Собираем бинарник для GPU:

`cd ..`

`sudo make`

- Запуск под GPU

`./main_gpu`

Можно сделать вывод в лог файл командой

`./main_gpu > gpu.log`



**Полезные материалы по теме:**

- How-to: OpenCL 1.2 on Intel HD Graphics (3. gen and newer) under Linux  
https://github.com/HPCE/hpce-2017-cw3/issues/53

- Примеры OpenCL от nvidia 
https://developer.nvidia.com/opencl


**ENG:**

Intel VS NVIDIA GPU OpenCL Matrix Multiply

A small project to compare the performance of computing on an integrated Intel video processor, an Intel central processor and an Nvidia video card.
The multiplication of the SGEMM matrix of the most basic type is implemented using OpenCL.


The code for the GPU implements the enumeration of all platforms and the execution of calculations on the first device of each platform.
The time of the program is measured. The number of launches can be changed. The first launch does not go to offset and is necessary for a "cold start".


**Build and Run:**

To run the calculations on an integrated video processor, use the OpenCL implementation for the Intel GPU - 
Beignet (https://www.freedesktop.org/wiki/Software/Beignet/).
We assemble, install according to the instructions from the link above.

- The NVIDIA video processor uses the CUDA Toolkit to run the calculations.

Install the cuda toolkit with:

`sudo apt-get install nvidia-cuda-toolkit`

You also need to install 'clinfo' to test the operation of OpenCL platforms and devices.

`sudo apt-get install clinfo`


- Build a binary for CPU:

`cd ./cpu`

`sudo make`

- Run under CPU

`./main_cpu`

You can make output to the log file with the command
./main_cpu> cpu.log

- Build a binary for the GPU:

`cd ..`

`sudo make`

- Run under GPU

`./main_gpu`

You can make output to the log file with the command

`./main_gpu> gpu.log`




**Useful materials on the topic:**

- How to: OpenCL 1.2 on Intel HD Graphics (3.gen and newer) under Linux https://github.com/HPCE/hpce-2017-cw3/issues/53

- OpenCL examples from nvidia at https: //developer.nvidia.com/opencl
