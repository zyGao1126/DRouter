#!/usr/bin/env bash

./Router --lef ./sample/sample.lef --def ./sample/sample.def --result ./result/result.def

# check memory leak
# valgrind --leak-check=yes ./Router --lef ./sample/sample.lef --def ./sample/sample.def --result ./result/result.def