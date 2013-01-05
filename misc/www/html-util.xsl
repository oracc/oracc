<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  exclude-result-prefixes="dc xl">

<xsl:variable name="lc" select="'abcdefghijklmnopqrstuvwxyzšŋ'"/>
<xsl:variable name="uc" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZŠŊ'"/>

<xsl:variable name="utf8" select="'ṣṢṭṬŋŊ'"/>
<xsl:variable name="html" select="'şŞţŢĝĜ'"/>

<xsl:variable name="subdig" select="'₀₁₂₃₄₅₆₇₈₉₊'"/>
<xsl:variable name="regdig" select="'0123456789x~'"/>
<xsl:variable name="empty"  select="''"/>

<xsl:template match="text()">
<!--  <xsl:message>html-util.xsl: text() = <xsl:value-of select="."/></xsl:message> -->
  <xsl:call-template name="html-text">
    <xsl:with-param name="text" select="."/>
  </xsl:call-template>
</xsl:template>

<xsl:template mode="fo" match="text()">
  <xsl:call-template name="fo-text">
    <xsl:with-param name="text" select="."/>
  </xsl:call-template>
</xsl:template>

<xsl:template mode="atf" match="text()">
  <xsl:call-template name="atf-text">
    <xsl:with-param name="text" select="."/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="html-cfgw">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="contains($text,'[')">
      <span class="cf">
        <xsl:call-template name="html-text">
	  <xsl:with-param name="text" select="substring-before($text,'[')"/>
        </xsl:call-template>
      </span>
      <span class="gw">
	<xsl:text> [</xsl:text>
        <xsl:call-template name="html-text">
	  <xsl:with-param name="text" 
		select="translate(substring-before(substring-after($text,'['),']'),
				  'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
        </xsl:call-template>
	<xsl:text>]</xsl:text>
      </span>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="html-text">
	<xsl:with-param name="text" select="$text"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="html-text">
  <xsl:param name="text"/>
  <xsl:value-of select="$text"/>
</xsl:template>

<xsl:template name="x-html-text">
  <xsl:param name="text"/>
  <xsl:call-template name="sub-dig">
<!--    <xsl:with-param name="text" select="translate($text,$utf8,$html)"/>	 -->
    <xsl:with-param name="text" select="$text"/>
    <xsl:with-param name="render" select="'XHTML'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="fo-text">
  <xsl:param name="text"/>
  <xsl:call-template name="sub-dig">
<!--    <xsl:with-param name="text" select="translate($text,$utf8,$html)"/>	-->
    <xsl:with-param name="text" select="$text"/>	
    <xsl:with-param name="render" select="'FO'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="atf-text">
  <xsl:param name="text"/>
  <xsl:call-template name="sz">
    <xsl:with-param name="t" select="translate(translate($text,$subdig,$regdig),
							'×','x')"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="sub-dig">
  <xsl:param name="text" select="."/>
  <xsl:param name="render"/>
  <xsl:variable name="char" select="substring($text,1,1)"/>
  <xsl:choose>
    <xsl:when test="string-length(translate($char,$subdig,$empty)) > 0">
      <xsl:value-of select="$char"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="string-length($char) > 0">
        <xsl:call-template name="sub">
	  <xsl:with-param name="text" select="translate($char,$subdig,$regdig)"/>
	  <xsl:with-param name="render" select="$render"/>
	</xsl:call-template>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="string-length($text) > 1">
    <xsl:call-template name="sub-dig">
      <xsl:with-param name="text" select="substring($text,2)"/>
      <xsl:with-param name="render" select="$render"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:variable name="quote-char"><xsl:text>'</xsl:text></xsl:variable>
<xsl:variable name="escaped-quote"><xsl:text>\'</xsl:text></xsl:variable>
<xsl:template name="escape-quotes">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="contains($text,$quote-char)">
      <xsl:variable name="pre" select="substring-before($text,$quote-char)"/>
      <xsl:variable name="post">
	<xsl:call-template name="escape-quotes">
	  <xsl:with-param name="text" 
		select="substring-after($text,concat($pre,$quote-char))"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($pre,$escaped-quote,$post)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="super-gloss">
  <xsl:param name="t" select="text()"/>
  <xsl:param name="render" select="'XHTML'"/>
  <xsl:param name="plain-text" select="'no'"/>
  <xsl:choose>
    <xsl:when test="contains($t,'{')">
      <xsl:call-template name="text">
        <xsl:with-param name="text" select="substring-before($t,'{')"/>
	<xsl:with-param name="render" select="$render"/>
      </xsl:call-template>
      <xsl:call-template name="sup">
	<xsl:with-param name="text" 
		select="substring-before(substring-after($t,'{'),
  	                                                  '}')"/>
	<xsl:with-param name="render" select="$render"/>
      </xsl:call-template>
      <xsl:call-template name="super-gloss">
        <xsl:with-param name="t" select="substring-after($t,'}')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="$plain-text = 'yes'">
      <xsl:value-of select="$t"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="text">
        <xsl:with-param name="text" select="$t"/>
	<xsl:with-param name="render" select="$render"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="text">
  <xsl:param name="text"/>
  <xsl:param name="render"/>
  <xsl:choose>
    <xsl:when test="$render='XHTML'">
      <xsl:call-template name="html-text">
	<xsl:with-param name="text" select="$text"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="$render='FO'">
      <xsl:call-template name="fo-text">
	<xsl:with-param name="text" select="$text"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="$render='ATF'">
      <xsl:call-template name="atf-text">
	<xsl:with-param name="text" select="$text"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>html-util.xsl:text: unhandled render type '<xsl:value-of select="$render"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sub">
  <xsl:param name="text"/>
  <xsl:param name="render"/>
  <xsl:choose>
    <xsl:when test="$render='XHTML'">
      <sub>
	<xsl:value-of select="$text"/>
      </sub>
    </xsl:when>
    <xsl:when test="$render='FO'">
      <fo:inline vertical-align="sub">
	<xsl:value-of select="$text"/>
      </fo:inline>
    </xsl:when>
    <xsl:when test="$render='ATF'">
      <xsl:value-of select="$text"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>html-util.xsl:sub: unhandled render type '<xsl:value-of select="$render"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sup">
  <xsl:param name="text"/>
  <xsl:param name="render"/>
  <xsl:choose>
    <xsl:when test="$render='XHTML'">
      <sup>
	<xsl:call-template name="html-text">
  	  <xsl:with-param name="text" select="$text"/>
	</xsl:call-template>
      </sup>
    </xsl:when>
    <xsl:when test="$render='FO'">
      <fo:inline vertical-align="super">
	<xsl:call-template name="fo-text">
  	  <xsl:with-param name="text" select="$text"/>
	</xsl:call-template>
      </fo:inline>
    </xsl:when>
    <xsl:when test="$render='ATF'">
      <xsl:text>{</xsl:text>
	<xsl:call-template name="atf-text">
  	  <xsl:with-param name="text" select="$text"/>
	</xsl:call-template>
      <xsl:text>}</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>html-util.xsl:sup: unhandled render type '<xsl:value-of select="$render"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sz">
  <xsl:param name="t"/>
  <xsl:choose>
    <xsl:when test="contains($t,'š')">
      <xsl:value-of select="substring-before($t,'š')"/>
      <xsl:text>sz</xsl:text>
      <xsl:call-template name="sz">
	<xsl:with-param name="t" select="substring-after($t,'š')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="contains($t,'Š')">
      <xsl:value-of select="substring-before($t,'Š')"/>
      <xsl:text>SZ</xsl:text>
      <xsl:call-template name="sz">
	<xsl:with-param name="t" select="substring-after($t,'Š')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$t"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
