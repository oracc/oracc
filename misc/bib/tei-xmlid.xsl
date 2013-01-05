<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
    xmlns="http://www.tei-c.org/ns/1.0"
    xmlns:tei="http://www.tei-c.org/ns/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="tei:listBibl">
  <xsl:variable name="last-id">
    <xsl:choose>
      <xsl:when test="count(*[@xml:id])>0">
	<xsl:for-each select="*">
	  <xsl:sort select="@xml:id"/>
	  <xsl:if test="position()=last()">
	    <xsl:value-of select="number(substring(@xml:id,2))"/>
	  </xsl:if>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="0"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="*[@xml:id]"/>
    <xsl:for-each select="*[not(@xml:id)]">
      <xsl:copy>
	<xsl:attribute name="xml:id">
	  <xsl:text>B</xsl:text>
	  <xsl:number format="000001" value="$last-id + position()"/>
	</xsl:attribute>
	<xsl:copy-of select="@*|*"/>
      </xsl:copy>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>