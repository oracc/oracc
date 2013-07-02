<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xl="http://www.w3.org/1999/xlink"
		xmlns:tei="http://www.tei-c.org/ns/1.0"
		xmlns="http://www.tei-c.org/ns/1.0"
		xmlns:oracc="http://oracc.org/ns/oracc/1.0">

<!-- @SUMMARY@= remove glossary entries that don't occur, 
     and texts that aren't lemmatized -->

<xsl:param name="version" select="1.0"/>
<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="tei:TEI">
  <xsl:choose>
    <xsl:when test="tei:text[@type='transliteration']">
      <xsl:if test="*/tei:body/tei:div1/tei:p/tei:s/tei:w/@lemma">
	<xsl:copy-of select="."/>
      </xsl:if>
    </xsl:when>
    <xsl:otherwise>
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:apply-templates/>
      </xsl:copy>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

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
