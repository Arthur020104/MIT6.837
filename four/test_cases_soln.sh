#!/bin/sh
# rm *.tga
# rm *.bmp

./a4soln -input scene01_plane.txt  -size 1600 1600 -output 1s.bmp
./a4soln -input scene02_cube.txt  -size 1600 1600 -output 2s.bmp
./a4soln -input scene03_sphere.txt  -size 1600 1600 -output 3s.bmp
./a4soln -input scene04_axes.txt  -size 1600 1600 -output 4s.bmp
./a4soln -input scene05_bunny_200.txt  -size 1600 1600 -output 5s.bmp
./a4soln -input scene06_bunny_1k.txt  -size 1600 1600 -output 6s.bmp
./a4soln -input scene07_shine.txt  -size 1600 1600 -output 7s.bmp
./a4soln -input scene08_c.txt -size 1600 1600 -output 8s.bmp
./a4soln -input scene09_s.txt -size 1600 1600 -output 9s.bmp
