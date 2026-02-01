#!/bin/sh
# rm *.tga
# rm *.bmp

./a5soln -input scene06_bunny_1k.txt  -size 1200 1200 -output 6a.bmp\
   -shadows -bounces 4 -jitter -filter
./a5soln -input scene10_sphere.txt  -size 1200 1200 -output 10a.bmp\
   -shadows -bounces 4 -jitter -filter
./a5soln -input scene11_cube.txt  -size 1200 1200 -output 11a.bmp\
 -shadows -bounces 4 -jitter -filter
./a5soln -input scene12_vase.txt  -size 1200 1200 -output 12a.bmp\
 -shadows -bounces 4 -jitter -filter

