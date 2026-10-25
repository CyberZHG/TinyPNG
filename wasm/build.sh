#!/usr/bin/env bash

emcmake cmake .. -B wasm -DTINY_PNG_BIND_ES=ON
(cd wasm && emmake make TinyPNGWASM)
