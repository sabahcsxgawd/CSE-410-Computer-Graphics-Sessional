cp ../../IOs/$1/scene.txt scene.txt
cp ../../IOs/$1/config.txt config.txt
rm stage*.txt z_buffer.txt out.bmp
./run.sh
diff --strip-trailing-cr stage1.txt ../../IOs/$1/stage1.txt
echo "stage1.txt passed"
diff --strip-trailing-cr stage2.txt ../../IOs/$1/stage2.txt
echo "stage2.txt passed"
diff --strip-trailing-cr stage3.txt ../../IOs/$1/stage3.txt
echo "stage3.txt passed"
diff --strip-trailing-cr z_buffer.txt ../../IOs/$1/z_buffer.txt
echo "z_buffer.txt passed"
diff --strip-trailing-cr out.bmp ../../IOs/$1/out.bmp
echo "out.bmp passed"