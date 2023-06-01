#!/bin/bash

gcc -Wall -Wextra -Werror -o hash hashmap.c
valgrind  --leak-check=full --track-origins=yes --show-leak-kinds=all ./hash

