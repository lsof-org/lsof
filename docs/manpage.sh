#!/bin/sh
echo "# Manpage" > manpage.md
echo "\`\`\`manpage" >> manpage.md
# nroff: render manpage
# col: -b remove backspace, -x use spaces instead of tabs
# cat: -s remove consecutive blank lines
nroff -man ../lsof.man | col -bx | cat -s >> manpage.md
echo "\`\`\`" >> manpage.md
