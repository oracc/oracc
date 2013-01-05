<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:key name="js-aka" match="jrnser" use="aka/text()"/>
<xsl:key name="js-key" match="jrnser" use="@key"/>

<xsl:template name="map-jrnser">
  <xsl:param name="js"/>
  <xsl:variable name="jsname" select="normalize-space($js)"/>
  <xsl:variable name="xmlid" select="ancestor-or-self::*[@xml:id]/@xml:id"/>
  <xsl:if test="string-length($jsname) > 0">
    <xsl:for-each select="document('/usr/local//lib/bib/jrnser.xml')">
      <xsl:variable name="key" select="key('js-aka',$jsname)"/>
      <xsl:choose>
        <xsl:when test="$key">
	  <xsl:value-of select="$key/@key"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:if test="count(key('js-key',$jsname)) = 0">
            <xsl:message><xsl:value-of select="$xmlid"/>: Unknown Journal or Series '<xsl:value-of 
                select="$jsname"/>'</xsl:message>
          </xsl:if>
          <xsl:value-of select="$jsname"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
