<xsl:stylesheet version="1.0" 
  xmlns:bib="http://oracc.org/ns/bib/1.0"
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:exslt="http://exslt.org/common"
  extension-element-prefixes="exslt"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="cites|bib:cites">
  <xsl:choose>
    <xsl:when test="@type='(' or @type='['">
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:when>
    <xsl:when test="@type='{'">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>ckey-namedate.xsl: unknown type <xsl:value-of 
      select="@type"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="cite|bib:cite">
  <xsl:choose>
    <xsl:when test="../@type='('">
      <!--
	  if this cite is the first in the series for a name, use the name
	    if the name is a multi, use the initial as well
	  always use the date
      -->
      <xsl:call-template name="dump"/>
    </xsl:when>
    <xsl:when test="../@type='['">
      <!-- 
	   only use the date
       -->
      <xsl:call-template name="dump"/>
    </xsl:when>
    <xsl:otherwise>
      <!-- 
	   always use name and date, because this should be an inline
	   singleton cite with no fence punctuation 
        -->
      <xsl:call-template name="dump"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
</xsl:template>

<xsl:template name="dump">
  <xsl:value-of select="id(@ref)/@bib:last"/>
  <xsl:text> </xsl:text>
  <xsl:value-of select="id(@ref)/@bib:date"/>
</xsl:template>

<xsl:template match="*|@*|text()|comment()|processing-instruction()">
  <xsl:copy>
    <xsl:apply-templates select="*|@*|text()|comment()|processing-instruction()"/>
  </xsl:copy>
</xsl:template>
  
</xsl:stylesheet>
