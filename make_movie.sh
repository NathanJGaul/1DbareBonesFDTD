#!/bin/sh
for i in sim.*; do
    [ -f "$i" ] || break
    eplot -r [0:200][-1:1] -M -P "$i" 2>/dev/null
    mv foo.png "$i".png
done

# combine individual frames into animated gif
convert -delay 20 -loop 0 sim.*.png movie.gif

# remove remaining frames
rm *.png