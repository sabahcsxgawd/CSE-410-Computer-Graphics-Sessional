clear
g++ -fsanitize=address point.cpp matrix.cpp triangle.cpp trial.cpp -o trial
./trial scene.txt config.txt