<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xl="http://www.w3.org/1999/xlink"
		xmlns:tei="http://www.tei-c.org/ns/1.0"
		xmlns="http://www.tei-c.org/ns/1.0"
		xmlns:oracc="http://oracc.org/ns/oracc/1.0">

<!-- @SUMMARY@= Create TEI &lt;listNym> from Corpus-Based Dictionary -->

<xsl:param name="version" select="1.0"/>
<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="tei:nym|tei:orth">
  <xsl:if test="@oracc:icount">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:if>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
