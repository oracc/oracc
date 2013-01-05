#!/bin/sh
if cd 01bld; then 
    find . -maxdepth 1 -type f -exec rm '{}' ';'
    rm -fr [PQX][0-9][0-9][0-9]
    rm -fr [a-z][a-z][a-z]
    rm -fr *-x-* lists/*
    cd ..
fi
cd 01tmp && (find . -maxdepth 1 -type f -exec rm '{}' ';' ; cd)
