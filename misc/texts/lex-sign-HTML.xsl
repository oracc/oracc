<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="lex:sign-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lex:group[@type='sign']">
  <div class="lex-sign" title="{@value}">
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='read']">
  <div class="lex-read" title="{@value}">
    <h2><xsl:value-of select="@value"/></h2>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='spel']">
  <p class="lex-spel" title="{@value}">
    <span class="lex-spel-tlit"><xsl:value-of select="@value"/></span>
    <xsl:text>: </xsl:text>
    <xsl:for-each select="lex:data">
      <xsl:apply-templates select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>; </xsl:text>
      </xsl:if>
    </xsl:for-each>
    <xsl:text>.</xsl:text>
  </p>
</xsl:template>

<xsl:template match="lex:data">
  <esp:link url="http://oracc.org/{@project}/{@sref}" site-name="{@project} on Oracc"><xsl:value-of select="@n"/>
  <xsl:text>, </xsl:text>
  <xsl:value-of select="@label"/></esp:link>
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
</xsl:template>

</xsl:stylesheet>
