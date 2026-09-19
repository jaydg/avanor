#!/bin/sh
#
# Regenerates the backdrop the macOS disk image window draws behind its
# icons - resources/dmg-background.png, and the -unsigned variant beside
# it. Both are committed; this only has to be run when the wording or the
# layout changes, the way resources/make-icon.sh is.
#
# Two of them, because the honest thing to say depends on the build. A
# signed and notarised image opens on a double click and has nothing to
# explain; an unsigned one is refused by Gatekeeper, and the window is
# the last place anybody reads before that happens. The Makefile picks
# whichever matches what it built, so the picture can never contradict
# what the user is about to see.
#
# The colours are the game's own - xCYAN as engine/global.h defines it,
# on the same near-black the icon is drawn on - so the window looks like
# the thing it installs.
#
# Sizes here are the create-dmg window and icon coordinates in the
# Makefile's dmg target. They are two halves of one layout: move an icon
# there and the arrow here points at nothing.
#
# Needs ImageMagick and the DejaVu fonts. Run from the repository root.

set -eu

out=resources/dmg-background.png
out_unsigned=resources/dmg-background-unsigned.png

# The window, and the two rows of icons Finder puts on top of it.
#
# Every x here is at or past 114. Finder will not place an icon nearer
# the left edge than that, and rather than clamp the one icon it shifts
# every icon in the window by the same amount to keep their spacing - so
# a single icon asked for at 70 slides the whole layout 44 to the right
# and leaves the arrow below pointing at nothing. Keep them all clear of
# it and Finder puts each exactly where it was asked to.
width=480
height=460
icon_row=130            # --icon "Avanor.app" / --app-drop-link
app_x=130
applications_x=350

ink='#00A6A6'           # xCYAN, as engine/global.h defines it
paper='#12161A'
dim='#6F8184'           # ink muted into the paper, for the small print

# fc-match answers with *something* whatever it is asked for, so a
# machine without DejaVu would quietly render these in whatever it does
# have and the committed files would change for no reason anyone could
# see in the diff. Ask, then check we were given what we asked for.
font_for() {
	file=$(fc-match -f '%{file}' "$1")

	case "$file" in
		*DejaVu*) ;;
		*)
			echo "make-background.sh: no DejaVu font for '$1'." >&2
			echo "  fontconfig offered: $file" >&2
			echo "  Install the DejaVu family and run this again -" >&2
			echo "  on macOS: brew install --cask font-dejavu" >&2
			exit 1
			;;
	esac

	echo "$file"
}

sans=$(font_for "DejaVu Sans")
sans_bold=$(font_for "DejaVu Sans:bold")

# The arrow runs between the two icons of the top row, stopping clear of
# both: 64-pixel icons centred on icon_row reach 32 to either side, and a
# little more is left so it points at the folder rather than into it.
arrow_from=$((app_x + 46))
arrow_to=$((applications_x - 58))
head=$((arrow_to + 16))

# ImageMagick stamps the hour it ran into the PNG, which would make
# every regeneration a diff with no visible change in it. -strip and
# excluding the date and time chunks leave the file a function of this
# script alone, so `git diff` after a run means the picture really moved.
plain='-strip -define png:exclude-chunk=date,time'

draw_common() {
	magick -size ${width}x${height} xc:"$paper" \
		-font "$sans_bold" -fill "$ink" -pointsize 17 \
		-gravity North -annotate +0+24 'Drag Avanor into Applications' \
		-stroke "$ink" -strokewidth 3 \
		-draw "line $arrow_from,$icon_row $arrow_to,$icon_row" \
		-stroke none -fill "$ink" \
		-draw "polygon $arrow_to,$((icon_row - 9)) $arrow_to,$((icon_row + 9)) $head,$icon_row" \
		-font "$sans" -fill "$dim" -pointsize 10 \
		-gravity South -annotate +0+18 \
			'Avanor is free software under the GPL, version 2 or later.' \
		$plain "$1"
}

# The signed image: nothing to warn about, so nothing is said.
draw_common "$out"

# The unsigned one. What it describes is the only way through on macOS 15
# and later, which withdrew the Control-click shortcut that used to open
# an unsigned application - System Settings is now the whole of it.
magick "$out" \
	-stroke "$dim" -strokewidth 1 -draw "line 60,330 420,330" \
	-stroke none \
	-font "$sans_bold" -fill "$ink" -pointsize 13 \
	-gravity North -annotate +0+350 'First launch' \
	-font "$sans" -fill "$dim" -pointsize 11 \
	-gravity North \
	-annotate +0+376 'macOS will refuse to open Avanor: it is not signed by a' \
	-annotate +0+392 'registered Apple developer. Open System Settings > Privacy' \
	-annotate +0+408 '& Security, scroll to the bottom, and click "Open Anyway".' \
	$plain "$out_unsigned"

echo "wrote $out and $out_unsigned"
