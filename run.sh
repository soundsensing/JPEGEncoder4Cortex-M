
gcc -o ssjpegencode -lm jpegencoder.c -I. tools/main.c
#gdb -ex run --args
./ssjpegencode tmpiqr3um4v.y output.jpg 32 64 y lowest 
