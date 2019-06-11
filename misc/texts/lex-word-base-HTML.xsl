<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:o="http://oracc.org/ns/oracc/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="esp lex">

<xsl:template match="lex:word-base-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lex:group[@type='word']">
  <div class="lex-word" title="{@value}" o:id="{@oid}" id="lexpronounce">
    <h3 class="word-base">Pronunciation Data</h3>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='base']">
  <div class="lex-base" title="{@value}">
    <h2><xsl:value-of select="@value"/></h2>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='spel']">
  <p class="lex-spel" title="{@value}">
    <span class="lex-spel-tlit"><xsl:value-of select="@value"/></span>
    <xsl:text>: </xsl:text>
    <xsl:for-each select="*">
      <xsl:apply-templates select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>; </xsl:text>
      </xsl:if>
    </xsl:for-each>
    <xsl:text>.</xsl:text>
  </p>
</xsl:template>

<xsl:template match="lex:group[@type='refs']">
  <xsl:variable name="srefs">
    <xsl:for-each select="lex:data/@sref">
      <xsl:value-of select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>+</xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="proj" select="lex:data[1]/@project"/>
  <a href="http://oracc.org/{$proj}/{$srefs}" title="{$proj} on Oracc">
    <xsl:value-of select="lex:data[1]/@n"/>
    <xsl:text> </xsl:text>
    <xsl:for-each select="lex:data/@label">
      <xsl:value-of select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>, </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </a>
</xsl:template>

</xsl:stylesheet>
