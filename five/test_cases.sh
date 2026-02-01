#!/bin/sh
# rm *.tga
# rm *.bmp

./a4 -input scene06_bunny_1k.txt  -size 1200 1200 -output 6.bmp\
   -shadows -bounces 4 -jitter -filter
./a4 -input scene10_sphere.txt  -size 1200 1200 -output 10.bmp\
   -shadows -bounces 4 -jitter -filter
./a4 -input scene11_cube.txt  -size 1200 1200 -output 11.bmp\
 -shadows -bounces 4 -jitter -filter
./a4 -input scene12_vase.txt  -size 1200 1200 -output 12.bmp\
 -shadows -bounces 4 -jitter -filter

