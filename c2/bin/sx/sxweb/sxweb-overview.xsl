<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:g="http://oracc.org/ns/gdl/1.0"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:include href="mcol.xsl"/>
<xsl:include href="sxweb-util.xsl"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="sl:signlist">
  <esp:page
      xmlns="http://www.w3.org/1999/xhtml"
      >
    <esp:name><xsl:value-of select="'Overview'"/></esp:name>
    <esp:title><xsl:value-of select="'Overview'"/></esp:title>
    <html>
      <head/>
      <body>
	<xsl:call-template name="mcol">
	  <xsl:with-param name="columns" select="'8'"/>
	  <xsl:with-param name="nodes" select="sl:sign"/>
	  <xsl:with-param name="class" select="'pretty'"/>
	</xsl:call-template>
      </body>
    </html>
  </esp:page>
</xsl:template>

</xsl:transform>
