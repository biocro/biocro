#!/bin/bash


R -d "valgrind --tool=memcheck --leak-check=full" --vanilla < checkCropGro.R >valgrindoutput.txt 2>&1
