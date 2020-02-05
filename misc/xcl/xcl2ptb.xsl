<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xst">

<xsl:output method="text" encoding="utf-8"
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
   indent="no"/>

<xsl:variable name="from-chars" select="' ()'"/>
<xsl:variable name="to-chars"   select="'_&lt;>'"/>

<xsl:template match="xtf:composite|xtf:transliteration">
  <xsl:text>( (REF </xsl:text>
  <xsl:value-of select="concat('(CODE &lt;', translate(@n,' ','_'),'>)')"/>
  <xsl:text>)&#xa;</xsl:text>
  <xsl:value-of select="concat('  (ID ', @xml:id, ')')"/>
  <xsl:text>)&#xa;&#xa;</xsl:text>
  <xsl:apply-templates select="/xtf:*/*/xcl:xcl/*/*/xcl:c[@type='sentence']"/>
</xsl:template>

<xsl:template match="xcl:c[@type='sentence']">
  <xsl:text>((</xsl:text>
  <xsl:value-of select="@tag"/>
  <xsl:apply-templates/>
  <xsl:text>)&#xa;</xsl:text>
  <xsl:value-of select="concat('  (ID ', translate(concat(/*/@n,': ',@label),' ()','_&lt;>'), ')')"/>
  <xsl:text>)</xsl:text>
  <xsl:text>&#xa;&#xa;</xsl:text>
</xsl:template>

<xsl:template match="xcl:*">
  <xsl:for-each select="ancestor::xcl:*">
    <xsl:text>  </xsl:text>
  </xsl:for-each>
  <xsl:choose>
    <xsl:when test="@tag">
      <xsl:choose>
	<xsl:when test="xcl:l">
	  <xsl:value-of select="concat('(', @tag, ' ', translate(xcl:l/xff:f/@form, $from-chars, $to-chars), ')')"/>
	  <xsl:value-of select="concat('(LEMMA *&lt;', 
					translate(xcl:l/xff:f/@cf, $from-chars, $to-chars), 
					'[', translate(xcl:l/xff:f/@gw, $from-chars, $to-chars), ']', 
					'>)')"/>
	  <xsl:value-of select="concat('(CODE &lt;', xcl:l/@ref, '>)')"/>
	</xsl:when>
	<xsl:when test="@type='leaf'">
	  <xsl:value-of select="concat('(', @tag, ' *', @txt, ')')"/>
	</xsl:when>
	<xsl:when test="count(*)=1 and xcl:c[@type='leaf'][string-length(@tag)=0]">
	  <xsl:value-of select="concat('(', @tag, ' *', */@txt, ')')"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="concat('(', @tag, ' ')"/>
	  <xsl:apply-templates/>
	  <xsl:text>)</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
  </xsl:choose>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
