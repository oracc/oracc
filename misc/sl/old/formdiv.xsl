<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:template name="form-div">
  <xsl:param name="caller"/>
  <div class="ogsl-form" id="ogsldiv">
    <form name="ogslform" id="ogslform" action="/ogsl/sl" method="post" target="_top">
      <input type="hidden" name="project" value="ogsl"/>
      <input type="hidden" name="caller" value="{$caller}"/>
      <p class="unisearch">
	<input type="submit" value="FIND:" name="x"/>
	<input type="text" size="10" name="k1" value=""/>
	<select name="ext" onmouseup="submit()">
	  <option selected="yes" value="">SIGN</option>
	  <option value="forms">FORMS</option>
	  <option value="h">HOMOPHONES</option>
	  <option value="c">COMPOUNDS</option>
	  <option value="cinit">INITIAL</option>
	  <option value="clast">FINAL</option>
	  <option value="contains">TIMES</option>
	  <option value="contained">CONTAINED</option>
	</select>
      </p>
    </form>
  </div>
</xsl:template>

</xsl:transform>
