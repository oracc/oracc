<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="dc xl">

<xsl:template name="TeX-cfgw">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="contains($text,'[')">
      <xsl:text>\cfgw{</xsl:text>
        <xsl:call-template name="text">
	  <xsl:with-param name="text" select="substring-before($text,'[')"/>
        </xsl:call-template>
      <xsl:text>}{</xsl:text>
      <xsl:call-template name="text">
	<xsl:with-param name="text" 
			select="translate(substring-before(substring-after($text,'['),']'),
				'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
      </xsl:call-template>
      <xsl:text>}</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="text">
	<xsl:with-param name="text" select="$text"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="TeX-super-gloss">
  <xsl:param name="t" select="text()"/>
  <xsl:param name="plain-text" select="'no'"/>
  <xsl:choose>
    <xsl:when test="contains($t,'{')">
      <xsl:call-template name="text">
        <xsl:with-param name="text" select="substring-before($t,'{')"/>
      </xsl:call-template>
      <xsl:call-template name="sup">
	<xsl:with-param name="text" 
		select="substring-before(substring-after($t,'{'),
  	                                                  '}')"/>
      </xsl:call-template>
      <xsl:call-template name="TeX-super-gloss">
        <xsl:with-param name="t" select="substring-after($t,'}')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="$plain-text = 'yes'">
      <xsl:value-of select="$t"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="text">
        <xsl:with-param name="text" select="$t"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="text">
  <xsl:param name="text"/>
  <xsl:value-of select="$text"/>
</xsl:template>

<xsl:template name="sub">
  <xsl:param name="text"/>
  <xsl:text>\dn{</xsl:text>
  <xsl:value-of select="$text"/>
  <xsl:text>}</xsl:text>
</xsl:template>

<xsl:template name="sup">
  <xsl:param name="text"/>
  <xsl:text>\up{</xsl:text>
  <xsl:value-of select="$text"/>
  <xsl:text>}</xsl:text>
</xsl:template>

</xsl:stylesheet>
