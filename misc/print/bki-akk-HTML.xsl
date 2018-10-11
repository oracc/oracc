<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:x="http://oracc.org/ns/xis/1.0"
		exclude-result-prefixes="c x"
		>
<xsl:output method="xml" encoding="utf-8"/>

<xsl:include href="g2-gdl-HTML.xsl"/>

<xsl:template match="c:articles">
  <html xmlns="http://www.w3.org/1999/xhtml">
    <head>
      <meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
      <style>
	.akk { font-style: italic }
      </style>
      <title>CMAWRO Akk Index</title>
    </head>
    <body>
      <xsl:apply-templates select=".//c:entry"/>
    </body>
  </html>
</xsl:template>
  
<xsl:template match="c:entry">
  <div class="entry">
    <p class="cfgwpos">
      <i><xsl:value-of select="c:cf"/></i>
      [<xsl:value-of select="c:gw"/>]
      (<xsl:value-of select="c:pos"/>)
    </p>
    <p>
      <xsl:for-each select="c:senses/c:sense">
	<xsl:choose>
	  <xsl:when test="@stem">
	    <xsl:value-of select="@stem"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:if test="not(c:pos = ancestor::c:pos)">
	      <xsl:value-of select="c:pos"/>
	    </xsl:if>
	  </xsl:otherwise>
	</xsl:choose>
	<xsl:text> </xsl:text>
	<xsl:value-of select="c:mng"/>
	<xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
      </xsl:for-each>
    </p>
    <p>
      <xsl:for-each select="c:senses/c:sense">
	<xsl:if test="@stem">
	  <xsl:text>[</xsl:text>
	  <xsl:value-of select="@stem"/>
	  <xsl:text>] </xsl:text>
	</xsl:if>
	<xsl:for-each select="c:forms/c:form">
	  <!--<xsl:value-of select="@n"/>-->
	  <xsl:apply-templates select="c:t/*"/>
	  <xsl:text> </xsl:text>
	  <xsl:for-each select="x:rr/x:r">
	    <xsl:choose>
	      <xsl:when test="starts-with(@label2,'vm_')">
		<xsl:value-of select="substring-after(@label2,'vm_')"/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="@label2"/>
	      </xsl:otherwise>
	    </xsl:choose>
	    <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	  </xsl:for-each>
	  <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	</xsl:for-each>
	<xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
      </xsl:for-each>
    </p>
    </div>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
