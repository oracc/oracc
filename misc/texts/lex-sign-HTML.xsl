<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:lsi="http://oracc.org/ns/lsi/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="lsi:sign-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lsi:group[@type='sign']">
  <div class="lsi-sign" title="{@value}">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lsi:group[@type='read']">
  <div class="lsi-read" title="{@value}">
    <h2><xsl:value-of select="@value"/></h2>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lsi:group[@type='spel']">
  <p class="lsi-spel" title="{@value}">
    <span class="lsi-spel-tlit"><xsl:value-of select="@value"/></span>
    <xsl:text>: </xsl:text>
    <xsl:apply-templates/>
    <xsl:text>.</xsl:text>
  </p>
</xsl:template>

<xsl:template match="lsi:data">
  <xsl:value-of select="/*/@n"/>
  <xsl:text>, </xsl:text>
  <xsl:value-of select="@label"/>
  <xsl:choose>
    <xsl:when test="@word">
      <xsl:value-of select="concat(' (',@word,'/',@base,')')"/>
    </xsl:when>
    <xsl:when test="@pos">
      <xsl:value-of select="concat(' (',@pos,')')"/>
    </xsl:when>
    <xsl:otherwise>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="count(following-sibling::*) > 0">
    <xsl:text>; </xsl:text>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
