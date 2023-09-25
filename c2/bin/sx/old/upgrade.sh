#!/bin/sh
../../../misc/sl/sl-upgrade.plx ogsl-for-upgrade.asl  >ogsl-upgraded.asl ; ./sx -i ogsl-upgraded.asl >new-ogsl.asl
echo ogsl upgraded as ogsl-upgraded.asl and sorted as new-ogsl.asl
