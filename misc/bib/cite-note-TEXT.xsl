<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:bib="http://oracc.org/ns/bib/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:param name="citesfile" 
         select="'/home/s/projects/tetrad/book/citekeys.xml'"/>

<xsl:variable name="cites" 
         select="document($citesfile)/citekeys/*"/>

<xsl:template match="cites">
  <xsl:choose>
    <xsl:when test="@type='[' or @type='('">
      <xsl:if test="not(@prepunct=' ')">
        <xsl:value-of select="@prepunct"/>
      </xsl:if>
      <xsl:if test="not(@nxtpunct=' ')">
        <xsl:value-of select="@nxtpunct"/>
      </xsl:if>
      <bnote><xsl:apply-templates/>.</bnote>
      <xsl:text> </xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@prepunct"/>
      <xsl:apply-templates/>
      <xsl:value-of select="@nxtpunct"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="cite">
  <xsl:variable name="ref" select="@ref"/>
  <xsl:choose>
    <xsl:when test="$ref='BAD'">
      <xsl:value-of select="text()"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="ckey" select="$cites[@bib:id=$ref]"/>
      <xsl:value-of select="$ckey/names"/>
      <xsl:text> </xsl:text>
      <xsl:value-of select="$ckey/abbrev"/>
      <xsl:if test="text()">
        <xsl:text> </xsl:text>
        <xsl:value-of select="text()"/>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="following-sibling::cite">
    <xsl:text>; </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="*|@*|text()|comment()|processing-instruction()">
  <xsl:copy>
    <xsl:apply-templates select="*|@*|text()|comment()|processing-instruction()"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
