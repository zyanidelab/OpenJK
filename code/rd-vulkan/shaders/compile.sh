#!/bin/bash
for filename in *.vert; do
    name=$(echo "$filename" | cut -f 1 -d '.')
    glslangValidator -V $filename
    srec_cat vert.spv -Binary -o "spirv/${name}_vert.cpp" -C-Array "${name}_vert_spv" -No-CONST
    rm vert.spv
done

for filename in *.frag; do
    name=$(echo "$filename" | cut -f 1 -d '.')
    glslangValidator -V $filename
    srec_cat frag.spv -Binary -o "spirv/${name}_frag.cpp" -C-Array "${name}_frag_spv" -No-CONST
    rm frag.spv
done
