include ../../oracc-am-rules.txt

SUBDIRS=

cplus=$(htmldir)/cuneifyplus

noinst_DATA = cuneiform_cache.pickle cuneify_interface.py \
	environment.py requirements.txt setup.py wsgi.py

environment.py: environment.py.in
	$(ORACCIFY) environment.py.in
	chmod u+w environment.py

install-data-hook:
	mkdir -p $(cplus)
	cp -a $(noinst_DATA) $(cplus)
	cp -a fonts $(cplus)
	chmod -R o+r $(cplus)
