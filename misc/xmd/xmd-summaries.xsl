<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xmd="http://oracc.org/ns/xmd/1.0"
		xmlns="http://oracc.org/ns/xmd/1.0"
		exclude-result-prefixes="xmd"
		>

<xsl:template match="/*">
  <summaries project="{@project}">
    <xsl:apply-templates/>
  </summaries>
</xsl:template>

<xsl:template match="xmd:xmd">
  <summary>
    <xsl:attribute name="xml:id">
      <xsl:value-of select="@xml:id"/>
    </xsl:attribute>
    <xsl:for-each select="xmd:cat">
      <id_text>
	<xsl:choose>
	  <xsl:when test="xmd:id_text">
	    <xsl:value-of select="xmd:id_text"/>
	  </xsl:when>
	  <xsl:when test="xmd:id_composite">
	    <xsl:value-of select="xmd:id_composite"/>
	</xsl:when>
	</xsl:choose>
      </id_text>
      <xsl:apply-templates/>
    </xsl:for-each>
  </summary>
</xsl:template>

<xsl:template match="xmd:designation|xmd:primary_publication
		     |xmd:museum_no|xmd:excavation_no|xmd:accession_no
		     |xmd:genre|xmd:subgenre|xmd:period
		     |xmd:provenience">
  <xsl:element name="{local-name()}">
    <xsl:value-of select="."/>
  </xsl:element>
</xsl:template>

<xsl:template match="*"/>

<xsl:template match="text()"/>

</xsl:stylesheet>
