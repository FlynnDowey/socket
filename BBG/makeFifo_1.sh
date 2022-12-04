#!/bin/bash

pipe_1=/tmp/fifo1

if [[ ! -p $pipe_1 ]]; then
    mkfifo $pipe_1
fi