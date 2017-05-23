#!/bin/bash
if [ "$#" -lt 1 ]; then
        echo "usage: n7t328IIpnwd_lynx.sh URL"
fi
lynx -nolist -display_charset=utf-8 -dump "$1" | iconv -c -f utf-8 -t utf-32 | n7t328IIpnwd
