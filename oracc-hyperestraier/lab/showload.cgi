#! /bin/sh

#================================================================
# relwords.cgi
# CGI script to show load average
#================================================================


# set variables
LANG=C ; export LANG
LC_ALL=C ; export LC_ALL
PATH="$PATH:/sbin:/usr/sbin:/usr/local/bin:/usr/local/sbin:.:.." ; export PATH
LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/usr/local/lib:.:..:../.." ; export LD_LIBRARY_PATH
progname="sizecheck"


# show header
printf 'Content-Type: text/plain\r\n'
printf '\r\n'


# show result
uptime |
grep 'load average' |
sed -e 's/.*load average: *//' -e 's/^ *[0-9.]*, *//' -e 's/, .*//'


# exit normally
exit 0



# END OF FILE
