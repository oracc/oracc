#!/bin/sh
../../../misc/sl/sl-upgrade.plx ogsl-for-upgrade.asl  >ogsl-upgraded.asl ; ./sx ogsl-upgraded.asl
echo ogsl upgraded as ogsl-upgraded.asl
