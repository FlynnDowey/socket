#!/bin/bash

pipe_2=/tmp/fifo_ImgName

if [[ ! -p $pipe_2 ]]; then
    mkfifo $pipe_2
fi
