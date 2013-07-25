<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:x="http://oracc.org/ns/xtf/1.0"
  xmlns:c="http://oracc.org/ns/xcl/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:template match="x:l">
  <xsl:value-of select="concat(@label, '. ')"/>
  <xsl:apply-templates/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="g:w">
  <xsl:apply-templates/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:d">
  <xsl:text>{</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
</xsl:template>

<xsl:template match="g:v|g:s|g:r">
  <xsl:call-template name="g-begin"/>
  <xsl:apply-templates/>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:b">
  <xsl:choose>
    <xsl:when test="g:o">
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:p">
  
</xsl:template>

<xsl:template match="g:q">
  <xsl:apply-templates select="*[1]"/>
  <xsl:text>(</xsl:text>
  <xsl:apply-templates select="*[2]"/>
  <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="g:a">
  <xsl:text>~</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:m">
  <xsl:text>@</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:n">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:for-each select="g:b">
        <xsl:apply-templates select="g:r"/>
	<xsl:if test="string-length(*[2])>0">
	  <xsl:text>(</xsl:text>
	  <xsl:apply-templates select="*[2]"/>
	  <xsl:apply-templates select="g:a|g:m"/>
	  <xsl:text>)</xsl:text>
	</xsl:if>
      </xsl:for-each>
     </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates select="g:r"/>
	<xsl:if test="string-length(*[2])>0">
	  <xsl:text>(</xsl:text>
	  <xsl:apply-templates select="*[2]"/>
	  <xsl:text>)</xsl:text>
	</xsl:if>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:apply-templates select="g:a|g:m"/>
</xsl:template>

<xsl:template match="g:c">
  <xsl:text>|</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>|</xsl:text>
</xsl:template>

<xsl:template match="g:g">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:o">
  <xsl:choose>
    <xsl:when test="@type='beside'"><xsl:text>.</xsl:text></xsl:when>
    <xsl:when test="@type='joining'"><xsl:text>+</xsl:text></xsl:when>
    <xsl:when test="@type='containing'"><xsl:text>×</xsl:text></xsl:when>
    <xsl:when test="@type='above'"><xsl:text>&amp;</xsl:text></xsl:when>
    <xsl:when test="@type='crossing'"><xsl:text>%</xsl:text></xsl:when>
    <xsl:when test="@type='opposing'"><xsl:text>@</xsl:text></xsl:when>
    <xsl:when test="@type='repeated'">
      <xsl:value-of select="concat(.,'x')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>gdl-OATF: <xsl:value-of select="@type"/> not handled</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:gg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:*">
  <xsl:message>gdl-OATF: gdl:<xsl:value-of select="name()"/> not handled</xsl:message>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="c:xcl"/>

<xsl:template match="x:protocols"/>

<xsl:template match="x:transliteration|x:composite|x:object|x:surface|x:column">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:nonx">
  <xsl:choose>
    <xsl:when test="starts-with(., '(')">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="x:*">
  <xsl:message>gdl-OATF: xtf:<xsl:value-of select="name()"/> not handled</xsl:message>
  <xsl:apply-templates/>  
</xsl:template>

<xsl:template name="g-begin">
  <xsl:choose>
    <xsl:when test="string-length(@g:o)>0">
      <xsl:value-of select="@g:o"/>
    </xsl:when>
    <xsl:when test="string-length(@g:ho)>0">
      <xsl:value-of select="'⸢'"/>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="g-end">
  <xsl:choose>
    <xsl:when test="string-length(@g:c)>0">
      <xsl:value-of select="@g:c"/>
    </xsl:when>
    <xsl:when test="string-length(@g:hc)>0">
      <xsl:value-of select="'⸣'"/>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="g-delim">
  <xsl:choose>
    <xsl:when test="@g:delim='--'">
      <xsl:text>-</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@g:delim"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
