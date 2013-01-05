<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:a="http://relaxng.org/ns/compatibility/annotations/1.0"
		xmlns:d="http://oracc.org/ns/d"
		xmlns:m="http://oracc.org/ns/m"
		xmlns:rng="http://relaxng.org/ns/structure/1.0"
		xmlns="http://oracc.org/ns/xpd/1.0"
		xmlns:xpd="http://oracc.org/ns/xpd/1.0"
		exclude-result-prefixes="a d m rng">

<xsl:output method="xml" omit-xml-declaration="yes" indent="yes"/>
<xsl:key name="defs" match="rng:define" use="@name"/>

<xsl:template match="rng:start">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="rng:ref">
  <xsl:for-each select="key('defs',@name)">
    <xsl:apply-templates/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="rng:element">
  <xsl:element name="{@name}">
    <xsl:apply-templates mode="attr" select="rng:attribute"/>
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template mode="attr" match="rng:attribute">
  <xsl:attribute name="{@name}">
    <xsl:apply-templates/>
  </xsl:attribute>
</xsl:template>

<xsl:template match="rng:attribute"/>

<xsl:template match="rng:choice">
  <xsl:choose>
    <xsl:when test="rng:value[@d:default]">
      <xsl:value-of select="rng:value[@d:default]"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="rng:value[1]"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="rng:data">
  <xsl:choose>
    <xsl:when test="@type='boolean'">
      <xsl:choose>
	<xsl:when test="ancestor::rng:attribute/@d:default">
	  <xsl:value-of select="ancestor::rng:attribute/@d:default"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>false</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="@type='NCName' or @type='anyURI'"/>
    <xsl:otherwise>
      <xsl:message>unhandled data type '<xsl:value-of select="@type"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="rng:value">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="rng:interleave">
  <xsl:apply-templates select="*"/>
</xsl:template>

<xsl:template match="rng:zeroOrMore">
  <!-- The child is an rnc:choice, so we just process all the children -->
  <xsl:apply-templates select="*/*"/>
</xsl:template>

<xsl:template match="rng:grammar|rng:div">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="rng:define|rng:text"/>

<xsl:template match="rng:*">
  <xsl:message>unhandled rng element '<xsl:value-of select="local-name(.)"/></xsl:message>
</xsl:template>

<xsl:template match="a:*"/>

<xsl:template match="text()"/>

</xsl:stylesheet>
