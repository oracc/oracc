<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:bib="http://oracc.org/ns/bib/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:key name="nm-aka" match="name" use="aka/text()"/>
<xsl:key name="nm-key" match="name" use="@key"/>

<xsl:template name="map-name">
  <xsl:param name="nm" select="text()"/>
  <xsl:variable name="name" select="normalize-space($nm)"/>
  <xsl:if test="string-length($name) > 0">
    <xsl:for-each select="document('/usr/local//lib/bib/names.xml')">
      <xsl:variable name="key" select="key('nm-aka',$name)"/>
      <xsl:choose>
        <xsl:when test="$key">
	  <xsl:value-of select="$key/@key"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:if test="count(key('nm-key',$name)) = 0">
            <xsl:message>Unknown Name '<xsl:value-of 
                select="$name"/>'</xsl:message>
          </xsl:if>
          <xsl:value-of select="$name"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:template name="name-attr">
  <xsl:param name="nmkey"/>
  <xsl:for-each select="document('/usr/local//lib/bib/names.xml')">
    <xsl:attribute name="bib:last">
      <xsl:value-of select="key('nm-key',$nmkey)/last"/>
    </xsl:attribute>
    <xsl:attribute name="bib:first">
      <xsl:value-of select="key('nm-key',$nmkey)/rest"/>
    </xsl:attribute>
    <xsl:attribute name="bib:init">
      <xsl:value-of select="key('nm-key',$nmkey)/init"/>
    </xsl:attribute>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
