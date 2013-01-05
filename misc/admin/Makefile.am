include ../../oracc-am-rules.txt

bin_SCRIPTS = full-backup.sh incr-backup.sh projlist.sh projsave-all.sh projsave-full.sh projsave-incr.sh allprojatf.sh allprojlist.sh backup-list.plx grepatf.sh oraccenv.sh proj-backup-list.plx projbackup.sh projlist-depth-first.plx projpublic.sh projsave-sub.sh projsave.sh pubfiles.sh subprojdirs.sh subproject.sh sudo-map-exec.sh sudo-map.sh userperm.sh wwwify.sh wwwperm.sh

projsave-incr.sh: projsave-incr.sh.in
	$(ORACCIFY) projsave-incr.sh.in

full-backup.sh: full-backup.sh.in
	$(ORACCIFY) full-backup.sh.in

projsave-full.sh: projsave-full.sh.in
	$(ORACCIFY) projsave-full.sh.in

incr-backup.sh: incr-backup.sh.in
	$(ORACCIFY) incr-backup.sh.in

projlist.sh: projlist.sh.in
	$(ORACCIFY) projlist.sh.in

projsave-all.sh: projsave-all.sh.in
	$(ORACCIFY) projsave-all.sh.in

EXTRA_DIST = \
	allprojatf.sh \
	allprojlist.sh \
	backup-list.plx \
	full-backup.sh.in \
	grepatf.sh \
	incr-backup.sh.in \
	oraccenv.sh \
	proj-backup-list.plx \
	projbackup.sh \
	projlist-depth-first.plx \
	projlist.sh.in \
	projpublic.sh \
	projsave-all.sh.in \
	projsave-full.sh.in \
	projsave-incr.sh.in \
	projsave-sub.sh \
	projsave.sh \
	pubfiles.sh \
	subprojdirs.sh \
	subproject.sh \
	sudo-map-exec.sh \
	sudo-map.sh \
	userperm.sh \
	wwwify.sh \
	wwwperm.sh
