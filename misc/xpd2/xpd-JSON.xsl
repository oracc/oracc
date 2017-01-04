<xsl:stylesheet 
    xmlns:oracc="http://oracc.org/ns/oracc/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    >

<xsl:output method="text" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:text>{&#xa;&#x9;"projectlist": [</xsl:text>
  <xsl:for-each select="//xpd:project">
    <xsl:apply-templates select="."/>
    <xsl:if test="not(position()=last())"><xsl:text> , </xsl:text></xsl:if>
<!--    <xsl:text>&#xa;</xsl:text> -->
  </xsl:for-each>
  <xsl:text>]&#xa;}&#xa;</xsl:text>
</xsl:template>

<xsl:template match="xpd:project">
<!--  <xsl:text>&#x9;"project": {&#xa;</xsl:text> -->
  <xsl:text>{&#xa;</xsl:text>
  <xsl:value-of select="concat('&#x9;&#x9;&quot;pathname&quot;: &quot;',@n,'&quot;,')"/>
  <xsl:value-of select="concat('&#xa;&#x9;&#x9;&quot;name&quot;: &quot;',xpd:name,'&quot;,')"/>
  <xsl:value-of select="concat('&#xa;&#x9;&#x9;&quot;abbrev&quot;: &quot;',xpd:abbrev,'&quot;,')"/>
  <xsl:text>&#xa;&#x9;&#x9;&quot;blurb&quot;: &quot;</xsl:text>
  <xsl:apply-templates select="xpd:blurb"/>
  <xsl:text>&quot;</xsl:text>
  <xsl:if test="not(position()=last())"><xsl:text>,</xsl:text></xsl:if>  
  <xsl:text>&#xa;&#x9;}</xsl:text>
</xsl:template>

<xsl:template match="xpd:blurb">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()">
  <xsl:variable name="str"/>
    <xsl:call-template name="jsonify">
      <xsl:with-param name="text" select="."/>
    </xsl:call-template>
    <xsl:value-of select="$str"/>
</xsl:template>

<xsl:variable name="quote-char"><xsl:text>"</xsl:text></xsl:variable>
<xsl:variable name="escaped-quote"><xsl:text>\"</xsl:text></xsl:variable>

<xsl:variable name="slash-char"><xsl:text>\</xsl:text></xsl:variable>
<xsl:variable name="escaped-slash"><xsl:text>\\</xsl:text></xsl:variable>

<xsl:template name="jsonify">
  <xsl:param name="text"/>
<!--  <xsl:message>escaped-slash = <xsl:value-of select="$escaped-slash"/></xsl:message> -->
  <xsl:variable name="slash-text">
    <xsl:call-template name="escape-slashes">
      <xsl:with-param name="text" select="translate($text,'&#xa;',' ')"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="quote-text">
    <xsl:call-template name="escape-quotes">
      <xsl:with-param name="text" select="$slash-text"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:value-of select="$quote-text"/>
</xsl:template>

<xsl:template name="escape-quotes">
  <xsl:param name="text"/>
<!-- <xsl:message>escape quotes called with text=<xsl:value-of select="$text"/></xsl:message> -->
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

<xsl:template name="escape-slashes">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="contains($text,$slash-char)">
      <xsl:variable name="pre" select="substring-before($text,$slash-char)"/>
      <xsl:variable name="post">
	<xsl:call-template name="escape-slashes">
	  <xsl:with-param name="text" 
		select="substring-after($text,concat($pre,$slash-char))"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($pre,$escaped-slash,$post)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xpd:br"><xsl:text> </xsl:text></xsl:template>

<xsl:template match="xpd:a">
  <xsl:text>&lt;a href=\"</xsl:text>
  <xsl:value-of select="@href"/>
  <xsl:text>\"></xsl:text>
  <xsl:apply-templates/>
  <xsl:text>&lt;/a></xsl:text>
</xsl:template>

</xsl:stylesheet>
