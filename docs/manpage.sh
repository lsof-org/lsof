#!/bin/sh
echo "# Manpage" > manpage.md
echo "\`\`\`manpage" >> manpage.md
nroff -man ../lsof.man | colcrt - | cat -s >> manpage.md
echo "\`\`\`" >> manpage.md