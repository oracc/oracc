## Process this file with automake to produce Makefile.in
##
#  Copyright(c) Pennsylvania Sumerian Dictionary and Steve Tinney
#  1997 under the GNU General Public License (see /psd/COPYING).
# 
#  $Id: Makefile.am,v 1.1 1998/11/05 03:00:29 s Exp s $

ACLOCAL_AMFLAGS=-I m4
#SUBDIRS = oracc-hyperestraier misc c doc
SUBDIRS = misc c doc
ORACCIFY = $(abs_top_builddir)/oracc-subst.sh $(DESTDIR)$(prefix)

fix-oracc-path.sh: fix-oracc-path.sh.in
	$(ORACCIFY) fix-oracc-path.sh.in
mac-config.sh: mac-config.in
	$(ORACCIFY) mac-config.sh.in
missing-in.sh: missing-in.sh.in
	$(ORACCIFY) missing-in.sh.in
oracc-build.sh: oracc-build.sh.in
	$(ORACCIFY) oracc-build.sh.in
oracc-configure.sh: oracc-configure.sh.in
	$(ORACCIFY) oracc-configure.sh.in
oracc-make-doc-am.sh: oracc-make-doc-am.sh.in
	$(ORACCIFY) oracc-make-doc-am.sh.in
oracc-make-misc-am.sh: oracc-make-misc-am.sh.in
	$(ORACCIFY) oracc-make-misc-am.in
oracc-rebuild.sh: oracc-rebuild.sh.in
	$(ORACCIFY) oracc-rebuild.sh.in

EXTRA_DIST=oracc-subst.sh oracc-am-rules.txt \
	missing-in.sh.in \
	oracc-build.sh.in \
	oracc-configure.sh.in \
	oracc-make-doc-am.sh.in \
	oracc-make-misc-am.sh.in \
	oracc-rebuild.sh.in
