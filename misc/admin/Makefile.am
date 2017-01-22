include ../../oracc-am-rules.txt

bin_SCRIPTS = full-backup.sh incr-backup.sh projlist.sh projsave-all.sh projsave-full.sh \
	projsave-incr.sh allprojatf.sh allprojlist.sh backup-list.plx grepatf.sh oraccenv.sh \
	proj-backup-list.plx projbackup.sh projlist-depth-first.plx projpublic.sh projsave-sub.sh \
	projsave.sh pubfiles.sh subprojdirs.sh subproject.sh sudo-map-exec.sh sudo-map.sh \
	userperm.sh wwwify.sh wwwperm.sh oraccenv-single.sh oracc-project-data.sh oracc-project-runtime.sh \
	oracc-install.sh oracc-install-funcs.sh oracc-install-server.sh oracc-install-single.sh \
	sync-runtimes.plx sync-update.sh oracc-index-runtime.sh proj-from-conf-func.sh proj-from-conf.sh \
	repo-lemm.plx all-hubs.sh portal-words.sh bootstrap.sh serve-status.sh \
	serve-index.sh serve-install.sh serve-project.sh serve-cron.plx

#data_DATA = usermode.xml
#
#clean-usermode:
#	rm -f usermode.xml
#
#usermode.xml: clean-usermode
#	./usermode.sh
#
#dmg.sh: dmg.sh.in
#	$(ORACCIFY) dmg.sh.in

projsave-incr.sh: projsave-incr.sh.in
	$(ORACCIFY) projsave-incr.sh.in

full-backup.sh: full-backup.sh.in
	$(ORACCIFY) full-backup.sh.in

projsave-full.sh: projsave-full.sh.in
	$(ORACCIFY) projsave-full.sh.in

incr-backup.sh: incr-backup.sh.in
	$(ORACCIFY) incr-backup.sh.in

oraccenv-single.sh: oraccenv-single.sh.in
	$(ORACCIFY) oraccenv-single.sh.in

oraccenv.sh: oraccenv.sh.in
	$(ORACCIFY) oraccenv.sh.in

projlist.sh: projlist.sh.in
	$(ORACCIFY) projlist.sh.in

projsave-all.sh: projsave-all.sh.in
	$(ORACCIFY) projsave-all.sh.in

serve-cron.plx: serve-cron.plx.in
	$(ORACCIFY) serve-cron.plx.in

serve-index.sh: serve-index.sh.in
	$(ORACCIFY) serve-index.sh.in

serve-install.sh: serve-install.sh.in
	$(ORACCIFY) serve-install.sh.in

serve-project.sh: serve-project.sh.in
	$(ORACCIFY) serve-project.sh.in

sync-runtimes.plx: sync-runtimes.plx.in
	$(ORACCIFY) sync-runtimes.plx.in

sync-update.sh: sync-update.sh.in
	$(ORACCIFY) sync-update.sh.in

wwwify.sh: wwwify.sh.in
	$(ORACCIFY) wwwify.sh.in

wwwperm.sh: wwwperm.sh.in
	$(ORACCIFY) wwwperm.sh.in

EXTRA_DIST = \
	allprojatf.sh \
	allprojlist.sh \
	backup-list.plx \
	dmg.sh.in \
	full-backup.sh.in \
	grepatf.sh \
	incr-backup.sh.in \
	oraccenv.sh.in \
	oraccenv-single.sh.in \
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
	usermode.sh \
	userperm.sh \
	wwwify.sh.in \
	wwwperm.sh.in
