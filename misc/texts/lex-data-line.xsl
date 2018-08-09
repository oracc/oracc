<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:template match="lex:data">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="line">
      <xsl:call-template name="line"/>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="lex:wp|lex:eq">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="lang"><xsl:value-of select="*/@lang[1]"/></xsl:attribute>
    <xsl:attribute name="form">
      <xsl:call-template name="phrase">
	<xsl:with-param name="p" select="."/>
      </xsl:call-template>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template name="line">
  <xsl:choose>
    <xsl:when test="lex:wp[1]">
      <xsl:call-template name="phrase">
	<xsl:with-param name="p" select="lex:wp[1]"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="lex:sv[1]">
      <xsl:call-template name="phrase">
	<xsl:with-param name="p" select="lex:sv[1]"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise/>
  </xsl:choose>
    <xsl:if test="lex:eq">
      <xsl:text> = </xsl:text>
      <xsl:call-template name="phrase">
	<xsl:with-param name="p" select="lex:eq[1]"/>
      </xsl:call-template>
    </xsl:if>
</xsl:template>

<xsl:template name="phrase">
  <xsl:param name="p"/>
  <xsl:for-each select="$p/*">
    <xsl:choose>
      <xsl:when test="@norm">
	<xsl:value-of select="@norm"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="@form"/>
      </xsl:otherwise>
    </xsl:choose>   
    <xsl:if test="not(position()=last())">
      <xsl:text> </xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
