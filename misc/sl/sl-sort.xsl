<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    version="1.0">

<xsl:param name="sort" select="'borger'"/>
<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="/*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:choose>
      <xsl:when test="$sort = 'mzl' or $sort = 'borger'">
	<xsl:for-each select="*">
	  <xsl:sort select="sl:sort/@mzl" data-type="number"/>
	  <xsl:sort select="sl:sort/@ogsl" data-type="number"/>
	  <xsl:copy-of select="."/>
	</xsl:for-each>
      </xsl:when>
      <xsl:when test="$sort = 'lak'">
	<xsl:for-each select="*">
	  <xsl:sort select="sl:sort/@lak" data-type="number"/>
	  <xsl:sort select="sl:sort/@ogsl" data-type="number"/>
	  <xsl:copy-of select="."/>
	</xsl:for-each>
      </xsl:when>
      <xsl:when test="$sort = 'sllha'">
	<xsl:for-each select="*">
	  <xsl:sort select="sl:sort/@sllha" data-type="number"/>
	  <xsl:sort select="sl:sort/@ogsl" data-type="number"/>
	  <xsl:copy-of select="."/>
	</xsl:for-each>
      </xsl:when>
    </xsl:choose>
  </xsl:copy>
</xsl:template>

</xsl:transform>
