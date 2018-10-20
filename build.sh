export CROSS_COMP_BASE=$HOME/opt/cross

export PATH=$PATH:$CROSS_COMP_BASE/bin

find -iname "*.m4" -exec sh -c 'for f; do m4 "$f" > "${f%.m4}"; done' sh {} \;

make $1
