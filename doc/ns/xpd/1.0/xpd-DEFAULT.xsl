<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns="http://oracc.org/ns/xpd/1.0" 
		xmlns:xf="http://www.w3.org/2002/xforms"
		xmlns:ev="http://www.w3.org/2001/xml-events"
		xmlns:xs="http://www.w3.org/2001/XMLSchema"
		xmlns:a="http://relaxng.org/ns/compatibility/annotations/1.0"
		xmlns:d="http://oracc.org/ns/d"
		xmlns:m="http://oracc.org/ns/m"
		xmlns:rng="http://relaxng.org/ns/structure/1.0"
		xmlns:xpd="http://oracc.org/ns/xpd/1.0"
		exclude-result-prefixes="a d m rng">

<xsl:output method="xml" omit-xml-declaration="yes" indent="yes"/>

<xsl:key name="refs" match="rng:define" use="@name"/>

<xsl:template match="rng:grammar">
  <xsl:apply-templates select="rng:start"/>
</xsl:template>

<xsl:template match="rng:start">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="rng:ref">
  <xsl:apply-templates select="key('refs',@name)"/>
</xsl:template>

<xsl:template match="rng:element">
  <xsl:element name="{@name}">
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template match="rng:attribute">
  <xsl:attribute name="{@name}">
    <xsl:choose>
      <xsl:when test="not(rng:value) and not(rng:data) or rng:data/@type='anyURI' or rng:data/@type='NCName'">
	<xsl:text/> <!-- no value or data is text content -->
      </xsl:when>
      <xsl:when test="rng:value">
	<xsl:value-of select="rng:value"/>
      </xsl:when>
      <xsl:when test="@d:default">
	<xsl:value-of select="@d:default"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:choose>
	  <xsl:when test="rng:choice/rng:value[@d:default]">
	    <xsl:value-of select="rng:choice/rng:value[@d:default]"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:message>xpd-DEFAULT.xsl: no default for attribute <xsl:value-of 
	    select="preceding-sibling::rng:attribute[@name='name']/rng:value"/></xsl:message>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:attribute>
</xsl:template>

<xsl:template match="rng:*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="a:*"/>

<xsl:template match="text()"/>

</xsl:stylesheet>
