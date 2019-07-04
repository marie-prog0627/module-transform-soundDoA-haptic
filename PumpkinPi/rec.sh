#! /bin/bash

arecord -vD hw:0,0 -c 2 -d 10 -r 48000 -f S16_LE test.wav

