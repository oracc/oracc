#!/bin/sh
echo '<unodf file="'$1'">'
unzip -p $1 styles.xml  | grep -v '^<?xml'
unzip -p $1 content.xml | grep -v '^<?xml'
echo '</unodf>'
