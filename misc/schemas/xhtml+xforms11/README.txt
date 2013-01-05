This directory file contains a first draft of an XHTML + XForms 1.1 RelaxNG Schema, expressed in RelaxNG compact syntax.

It relies on the XForms 1.1 RNC schema being located in ../xforms11.

It relies on James Clark's RNC being located in ../../xhtml.rnc.
The XHTML schema is included with editors such as oXygen and Emacs.
It has been tested with this version:
  http://svn.savannah.gnu.org/viewvc/emacs/emacs/etc/schema/xhtml.rnc?revision=1.1.4.3&view=markup

Here is a list of the files included, and a brief description of what they do:

* xhtml+xforms.rnc
This is the RNC schema to associate with XHTML+XForms documents.

* xhtml-xforms-repeat-attributes.rnc
Optional module which adds the XForms repeat attributes to a small number of elements in XHTML and XForms.

* schemas.xml
A sample Locating Rules file binding the XHTML+XForms11 to this RNC schema.

Leigh Klotz
Leigh.Klotz@xerox.com
January 13, 2010
