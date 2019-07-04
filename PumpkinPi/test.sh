#! /bin/bash

export ALSADEV=plughw:0,0
JCONF=~/julius-kit/grammar-kit-master/testmic.jconf

julius -C $JCONF -48 -nostrip -charconv SJIS UTF-8 

#julius -C $JCONF -48 -quiet -nolog -progout 
#julius -C ~/julius-kit/dictation-kit-master/main.jconf -demo -48



