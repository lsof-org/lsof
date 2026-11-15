#!/bin/sh
cd "$(dirname "$0")" || exit 1

echo "# Manpage" > manpage.md
echo "\`\`\`manpage" >> manpage.md
# nroff: render manpage
# col: -b remove backspace, -x use spaces instead of tabs
# cat: -s remove consecutive blank lines
# GROFF_NO_SGR: emit overstrikes instead of SGR escapes, so col can remove
# them cleanly; otherwise col strips the escape introducer but leaves the
# SGR parameters (e.g. "4m") in the output.
# Generate lsof.man from Lsof.8 so the docs never use a stale build artifact.
(cd .. && soelim Lsof.8) > ../lsof.man
GROFF_NO_SGR=1 nroff -man ../lsof.man | col -bx | cat -s >> manpage.md
echo "\`\`\`" >> manpage.md
