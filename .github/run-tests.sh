#!/bin/sh
#
# What CI runs once a build is standing. Called with CONFIGURATION set to
# the make flags that produced the binary, so this knows which one to run.
#
# Only a default build can be tested here. Every mode calls vInit() before
# it does anything, and notcurses will not open without a terminal - under
# Actions there is none, and it aborts. The default backend is plain ANSI
# on a file descriptor and does not care, so it runs headless, and the
# game logic it exercises is the same either way: the two backends differ
# only in how the screen is drawn.
#
# Nothing here reads the keyboard on purpose, but --test polls for a key
# to see whether it has been told to stop, and treats end-of-input as
# "stop". /dev/zero never ends, so it keeps going until the timeout.

set -eu

case "${CONFIGURATION:-}" in
	*debug=1*) avanor=./avanor-d ;;
	*)         avanor=./avanor   ;;
esac

# The soak below is meant to be cut off - it has no end of its own. Only
# the timeout killing it, or a clean exit, counts as passing; anything
# else is the crash we are looking for.
SOAK_SECONDS=60

# macOS ships no timeout(1) - it is GNU coreutils, which Homebrew installs
# with every binary prefixed, so there it is gtimeout.
if command -v timeout >/dev/null 2>&1; then
	timeout_cmd=timeout
elif command -v gtimeout >/dev/null 2>&1; then
	timeout_cmd=gtimeout
else
	echo "neither timeout nor gtimeout found - install GNU coreutils" >&2
	exit 1
fi

# Where each run's stderr is caught so it can be looked at afterwards.
complaints=$(mktemp)
trap 'rm -f "$complaints"' EXIT

# Content faults - a Lua file naming something nothing declares, a
# handler that will not run - are reported on stderr and otherwise let
# the game carry on, because a player would rather have a world with one
# thing wrong in it than no world at all. CI has no such excuse: a run
# that says anything about the world has found a bug, so it fails here.
run() {
	echo "==> $*"
	"$@" </dev/zero >/dev/null 2>"$complaints" || {
		status=$?
		cat "$complaints" >&2
		return $status
	}

	if [ -s "$complaints" ]; then
		echo "the engine complained about the content:" >&2
		cat "$complaints" >&2
		return 1
	fi
}

# One location written and read back.
run "$avanor" --test-cereal

# A world saved in one process and restored in another.
run "$avanor" --test-save
run "$avanor" --test-load

# Two scripted worlds played to their end. These run the scheduler, the
# AI, combat and the whole Lua content stack for a few hundred thousand
# turns, and answer 1 if the scenario could not be set up at all.
run "$avanor" --simulate orc_attack --arg 1 -s 1
run "$avanor" --simulate mushroom_errand --arg 2 -s 1

# The world left to run itself for a while, with no hero in it.
echo "==> $avanor --test -s 4711 (${SOAK_SECONDS}s)"
status=0
"$timeout_cmd" "$SOAK_SECONDS" "$avanor" --test -s 4711 </dev/zero >/dev/null \
	2>"$complaints" || status=$?

# 124 is timeout(1) saying it had to kill it, which is the world still
# turning when time ran out - the outcome we want.
if [ "$status" -ne 0 ] && [ "$status" -ne 124 ]; then
	cat "$complaints" >&2
	echo "soak died with exit $status" >&2
	exit 1
fi

if [ -s "$complaints" ]; then
	echo "the engine complained about the content:" >&2
	cat "$complaints" >&2
	exit 1
fi

echo "all tests passed"
