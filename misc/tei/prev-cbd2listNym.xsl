<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xl="http://www.w3.org/1999/xlink"
		xmlns:tei="http://www.tei-c.org/ns/1.0"
		xmlns="http://www.tei-c.org/ns/1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		exclude-result-prefixes="c">

<!-- @SUMMARY@= Create TEI &lt;listNym> from Corpus-Based Dictionary -->

<xsl:param name="version" select="1.0"/>
<xsl:param name="project"/>
<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:key name="nyms" match="c:cf" use="text()"/>

<xsl:template match="/">
  <listNym>
    <xsl:apply-templates/>
  </listNym>
</xsl:template>

<xsl:template match="c:cf">
  <xsl:if test="generate-id()=generate-id(key('nyms',text()))">
    <nym>
      <xsl:attribute name="xml:id">
	<xsl:value-of select="concat('nym.',generate-id())"/>
      </xsl:attribute>
      <form><xsl:value-of select="text()"/></form>
    </nym>
  </xsl:if>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
