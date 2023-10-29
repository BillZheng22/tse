#!/bin/bash

VALGRIND = valgrind --leak-check=full --show-leak-kinds=all -s

valgrind: crawler
    $(VALGRIND) crawler https://thayer.github.io/engs50/ pages 1