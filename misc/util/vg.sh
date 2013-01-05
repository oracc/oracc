#!/bin/sh
/usr/bin/valgrind --track-origins=yes --leak-check=full --show-reachable=yes $*
