clear
rm Output1*.bmp
g++ -fsanitize=address 1905118_main.cpp -o 1905118.out -lGL -lGLU -lglut
./1905118.out