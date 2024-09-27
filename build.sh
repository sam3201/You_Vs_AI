#!/bin/bash

if ! command -v pkg-config &> /dev/null
then
    echo "pkg-config could not be found. Please install it."
    exit 1
fi

gcc -w -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL utils/Raylib/libraylib.a You_Vs_Ai.c -o You_Vs_Ai 
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi

lldb ./You_Vs_Ai
echo "Compilation successful." 
lldb ./You_Vs_Ai 
rm You_Vs_Ai 
