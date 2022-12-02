#!/bin/bash

pipe_2=/tmp/fifo2

if [[ ! -p $pipe_2 ]]; then
    mkfifo $pipe_2
fi
