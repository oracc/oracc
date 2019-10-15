<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:x="http://oracc.org/ns/xis/1.0"
		exclude-result-prefixes="c g x"
		>


<xsl:output method="xml" encoding="UTF-8"/>
<xsl:template match="c:entries">
  <html xmlns="http://www.w3.org/1999/xhtml" xmlns:c="http://oracc.org/ns/cbd/1.0" xmlns:g="http://oracc.org/ns/gdl/1.0">
    <head>
      <meta charset="utf-8"/>
      <title><xsl:value-of select="@title"/></title>
      <link rel="stylesheet" type="text/css" href="/css/chresto-index.css"/>
      <link rel="stylesheet" type="text/css" href="chresto-index.css"/>
    </head>
    <body>
      <xsl:for-each select="c:entry">
	<p>
	  <span class="headword"><xsl:value-of select="c:cf"/></span>
	  <xsl:text>&#xa0;</xsl:text>
	  <xsl:for-each select=".//x:rr">
	    <xsl:sort select="@c" data-type="number"/>
	    <span class="text"><xsl:value-of select="@name"/></span>
	    <xsl:text>&#xa0;</xsl:text>
	    <xsl:for-each select="x:r">
	      <span class="line"><xsl:value-of select="@label1"/></span>
	      <xsl:if test="not(position()=last())"><xsl:text>, </xsl:text></xsl:if>
	    </xsl:for-each>
	    <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	  </xsl:for-each>
	  <xsl:text>.</xsl:text>
	</p>
      </xsl:for-each>
    </body>
  </html>
</xsl:template>

</xsl:stylesheet>
