<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       version="1.0" 
	       xmlns="http://oracc.org/ns/xmd/1.0"
	       xmlns:x="http://oracc.org/ns/xmd/1.0"
	       >

<xsl:template match="x:record">
  <xsl:copy>
    <designation>
    <xsl:choose>
      <xsl:when test="string-length(x:museum_no)>0">
	<xsl:value-of select="x:museum_no"/>
      </xsl:when>
      <xsl:when test="string-length(x:accession_no)>0">
	<xsl:value-of select="x:accession_no"/>
      </xsl:when>
      <xsl:when test="string-length(x:excavation_no)>0">
	<xsl:value-of select="x:excavation_no"/>
      </xsl:when>
      <xsl:when test="string-length(x:publication_history)>0">
	<xsl:value-of select="x:publication_history"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="x:primary_publication"/>
      </xsl:otherwise>
    </xsl:choose>
    </designation>
    <xsl:apply-templates select="*[not(local-name()='designation')]"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
