#!/bin/bash

./dcc < $1 > me.asm
cat defs.asm >> me.asm

cd spim
./spim -file ../me.asm