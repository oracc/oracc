<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:exslt="http://exslt.org/common"
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="exslt">

<xsl:template name="format-biblist">
  <xsl:param name="nl" select="*"/>
  <xsl:variable name="sorted">
    <xsl:call-template name="sortbib">
      <xsl:with-param name="nodes" select="$nl"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:for-each select="exslt:node-set($sorted)/bibWrapper">
    <xsl:variable name="same-last-auth">
      <xsl:call-template name="calc-same-auth"/>
    </xsl:variable>
    <xsl:variable name="index"><xsl:value-of select="@index"/></xsl:variable>
    <xsl:variable name="prv-index"><xsl:value-of 
		select="preceding-sibling::*[1]/@index"/>
    </xsl:variable>
    <xsl:if test="not(string($nl[number($index)]) 
		      = string($nl[number($prv-index)]))">
      <xsl:for-each select="$nl[number($index)]">
        <xsl:call-template name="format-biblStruct">
          <xsl:with-param name="same-auth" select="$same-last-auth"/>
        </xsl:call-template>
      </xsl:for-each>
    </xsl:if>
  </xsl:for-each>
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
