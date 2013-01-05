<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xl="http://www.w3.org/1999/xlink"
		xmlns:tei="http://www.tei-c.org/ns/1.0"
		xmlns="http://www.tei-c.org/ns/1.0"
		>
<!-- @SUMMARY@= Insert @nymRef and @ref attributes in &lt;teiCorpus> document. -->

<xsl:output method="xml" indent="yes" encoding="utf-8"/>
<xsl:key name="pers" match="tei:person" use="tei:persName/@n"/>
<xsl:key name="nyms" match="tei:nym" use="tei:form"/>

<xsl:template match="tei:forename">
  <xsl:if test="not(ancestor::tei:person)">
    <xsl:variable name="nymKey" select="substring-before(@n,'[')"/>
    <xsl:variable name="nymRef" select="key('nyms',$nymKey)/@xml:id"/>
    <xsl:variable name="persRef" select="key('pers',@n)/@xml:id"/>
    <xsl:copy>
      <xsl:copy-of select="@*[not(self::n)]"/>
      <xsl:if test="string-length($nymRef)>1">
	<xsl:attribute name="nymRef"><xsl:value-of select="$nymRef"/></xsl:attribute>
      </xsl:if>
      <xsl:if test="string-length($persRef)>1">
	<xsl:attribute name="ref"><xsl:value-of select="$persRef"/></xsl:attribute>
      </xsl:if>
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
