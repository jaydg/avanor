#!/bin/sh
#
# Regenerates resources/avanor.ico from the game's own title screen.
#
# The icon is the "A" of the logo in Main.cpp's logo_text[] - the first
# nine columns of its eight rows - drawn in xCYAN, the colour the title
# screen draws it in. Two renderings go into the one file: the large
# sizes keep the characters the logo is built from, and the small ones
# keep only the letterform, because at 16 pixels the characters are mud.
#
# Needs ImageMagick and icotool (icoutils). Run from the repository root.

set -eu

out=resources/avanor.ico
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

font=$(fc-match -f '%{file}' "DejaVu Sans Mono:bold")
ink='#00A6A6'        # xCYAN, as engine/global.h defines it
paper='#12161A'

# The letter, taken from the logo rather than retyped.
sed -n '/^const char\* logo_text/,/MSG_WHITE/p' Main.cpp \
	| grep '^    "' | sed 's/^    "//; s/",\?$//' | cut -c1-9 > "$work/A.txt"

# Large: the characters themselves, rows tightened so the letter is
# roughly square rather than terminal-shaped.
magick -background "$paper" -fill "$ink" -font "$font" -pointsize 64 \
	-interline-spacing -22 label:@"$work/A.txt" "$work/text.png"
magick "$work/text.png" -resize 232x232 -background "$paper" \
	-gravity center -extent 256x256 "$work/large.png"

# Small: one filled block per character cell - the same shape, at a
# detail level a small icon can actually hold.
awk -v cell=24 '{
	for (x = 0; x < 9; x++)
		if (substr($0, x + 1, 1) != " " && substr($0, x + 1, 1) != "")
			printf "rectangle %d,%d %d,%d\n", x*cell, NR1*cell, (x+1)*cell-1, (NR1+1)*cell-1
} { NR1++ }' "$work/A.txt" > "$work/block.mvg"

magick -size 216x192 xc:"$paper" -fill "$ink" -draw "@$work/block.mvg" "$work/block.png"
magick "$work/block.png" -resize 232x232 -background "$paper" \
	-gravity center -extent 256x256 "$work/small.png"

for s in 256 128 64 48; do magick "$work/large.png" -resize ${s}x${s} "$work/i-$s.png"; done
for s in 32 16;        do magick "$work/small.png" -resize ${s}x${s} "$work/i-$s.png"; done

icotool -c -o "$out" "$work"/i-256.png "$work"/i-128.png "$work"/i-64.png \
	"$work"/i-48.png "$work"/i-32.png "$work"/i-16.png

cp "$work/i-256.png" resources/avanor-256.png

echo "wrote $out"
