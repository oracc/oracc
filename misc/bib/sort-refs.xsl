<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:exslt="http://exslt.org/common"
  extension-element-prefixes="exslt"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="sort-bib.xsl"/>

<xsl:template match="reflist">
  <xsl:param name="nl" select="*"/>
  <xsl:variable name="sorted">
    <xsl:call-template name="sortbib"/>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="exslt:node-set($sorted)/bibWrapper">
      <xsl:variable name="same-auth">
        <xsl:call-template name="calc-same-auth"/>
      </xsl:variable>
      <xsl:variable name="index" select="@index"/>
      <xsl:for-each select="$nl[number($index)]">
        <xsl:copy>
          <xsl:attribute name="same-auth"><xsl:value-of select="$same-auth"/></xsl:attribute>
          <xsl:copy-of select="@*"/>
          <xsl:copy-of select="*"/>
        </xsl:copy>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template name="calc-same-auth">
  <xsl:variable name="prv" select="preceding-sibling::*[1]"/>
  <xsl:choose>
    <xsl:when test="@akey1 = $prv/@akey1 
                    and @akey2 = $prv/@akey2
                    and @akey3 = $prv/@akey3 
                    and @akey4 = $prv/@akey4">
       <xsl:value-of select="'t'"/>
    </xsl:when>
    <xsl:otherwise>
       <xsl:value-of select="'f'"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
