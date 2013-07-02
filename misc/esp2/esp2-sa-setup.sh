#!/bin/sh
if [ ! -r ~/oracc-sites/esp2/esp2.sh ]; then 
    echo esp2-sa-setup.sh: no such file ~/oracc-sites/esp2/esp2.sh
    exit 1
fi
chmod +x ~/oracc-sites/esp2/esp2.sh ~/oracc-sites/esp2/*.plx
