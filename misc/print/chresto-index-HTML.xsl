<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:ex="http://exslt.org/common"
		extension-element-prefixes="ex">

<xsl:include href="html-util.xsl"/>

<xsl:output method="xml" indent="yes"/>
<xsl:variable name="quot"><xsl:text>'</xsl:text></xsl:variable>
<xsl:template match="/">
  <html>
    <head>
      <link rel="stylesheet" type="text/css" href="/css/chresto-index.css"/>
    </head>
    <body>
      <xsl:apply-templates/>
    </body>
  </html>
</xsl:template>

<xsl:template match="c:entry">
  <xsl:if test="c:r">
    <p>
      <span class="headword">
	<xsl:call-template name="super-gloss">
	  <xsl:with-param name="t" select="c:cf/@literal|c:cf"/>
	  <xsl:with-param name="plain-text" select="'yes'"/>
	</xsl:call-template>
      </span>
      <xsl:text> </xsl:text>
      <xsl:if test="c:senses">
	<xsl:text>“</xsl:text>
	<xsl:variable name="sense" select="c:senses/c:sense[1]/c:mng"/>
	<xsl:choose>
	  <xsl:when test="contains($sense,',')">
	    <xsl:value-of select="substring-before($sense,',')"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="$sense"/>
	  </xsl:otherwise>
	</xsl:choose>	  
	<xsl:text>”</xsl:text>
	<xsl:text> </xsl:text>
      </xsl:if>
      <xsl:variable name="sorted-r">
	<xsl:for-each select="c:r">
	  <xsl:sort select="c:text/@sort" data-type="number"/>
	  <xsl:sort select="@lref" data-type="number"/>
	  <xsl:sort select="@wref" data-type="number"/>
	  <xsl:copy-of select="."/>
	</xsl:for-each>
      </xsl:variable>
      <xsl:for-each select="ex:node-set($sorted-r)/*">
	<xsl:choose>
	  <xsl:when test="not(@tref = preceding-sibling::*[1]/@tref)">
	    <xsl:if test="count(preceding-sibling::c:r)>0">
	      <xsl:text>; </xsl:text>
	    </xsl:if>
	    <span class="text"><xsl:value-of select="c:text"/></span>
	    <xsl:text>&#xa0;</xsl:text>
	    <span class="line"><xsl:value-of select="c:line"/></span>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:if test="not(c:line = preceding-sibling::*[1]/c:line)">
	      <xsl:text>, </xsl:text>
	      <span class="line"><xsl:value-of select="c:line"/></span>
	    </xsl:if>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:for-each>
      <xsl:text>.</xsl:text>
    </p>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>