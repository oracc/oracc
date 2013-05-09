GDF implementation

1) Creation
===========

All GDF data lives in the oracc directory in a subdirectory 
'ood/PROJECT/data', a physical directory owned by the project.
So, for penn GDF there is a directory 

  @@ORACC@@/ood/penn/data

A soft link from the penn home account is made from penn/03ood to 
ood/penn/data, and each dataset is then housed in its own 
directory beneath that.

A soft link is also needed from PROJECT/02www/data to 
@@ORACC@@/PROJECT/data.

2) Installation and Backup
==========================

There is no GDF installation procedure, because GDF data is stored
in the live space from which it is served.

GDF data is not included in the nightly full/incremental backups.

3) Files
========

GDF requires one file, gdf.xml, to be present in the directory.

Other optional files are named for the project:

      PROJECT.xml -- the XML data (even this is optional; 
      		     	     	   see penn/data/cbs)
      PROJECT.xsl -- XSL styling for the data, if present, this
      		     applied to an entry when it is being 
		     delivered
      PROJECT.css -- CSS styling for the data, if present, this
      		     should be referenced in the HTML HEAD for
		     the data
      PROJECT.js  -- Javascript for the data, if present, this
      		     should be referenced in the HTML HEAD for
		     the data
      PROJECT.sh  -- a script which returns an entry for the
      		     data (see penn/data/cbs for an example)

    FUTURE POSSIBILITIES:

      index.html  -- a home page for the data

      wrapper.html-- an XHTML file containing one <gdf:entry/>
      		     element; the data for an entry is inserted
		     at that point.  The following elements 
		     may also be given:

		     	 <gdf:dataset-title/>
			 <gdf:entry-title/>
			 <gdf:prev-uri/>
			 <gdf:next-uri/>
		     
      esp 	  -- a portal for the data

3) Update
=========

The update script 'gdf-update.sh' is run to update the EST index.
If there is an script named '[PROJECT]-update.sh' it is run 
instead; that script can call gdf-update.sh at whatever point is
required.

4) URIs
=======

Any GDF dataset must provide a gdf.xml file which is used to create the index.html
for the dataset.

URIs constructed of the dataset URI followed by an ID default to returning the XML.

     /contrib/data/obsipn/id2987

Adding /html requests an HTML formatted version of the entry data.

     /contrib/data/obsipn/id2987/html

