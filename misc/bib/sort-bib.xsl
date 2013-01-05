<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:bib="http://oracc.org/ns/bib/1.0"
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:exslt="http://exslt.org/common"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  extension-element-prefixes="exslt">

<xsl:include href="map-name.xsl"/>

<xsl:key name="names" match="name" use="aka/text()"/>

<xsl:template name="sortbib">
  <xsl:param name="nodes" select="*"/>
  <xsl:variable name="sortbib-keys-rtf">
    <xsl:call-template name="sortbib-mk-keys">
      <xsl:with-param name="nodes" select="$nodes"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:for-each select="exslt:node-set($sortbib-keys-rtf)/bibWrapper">
    <xsl:sort select="@akey1"/>
    <xsl:sort select="@akey2"/>
    <xsl:sort select="@akey3"/>
<!--    <xsl:sort select="@akey4"/> -->
    <xsl:sort select="@dkey" data-type="number"/>
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:attribute name="ahash">
	<xsl:value-of select="concat(@akey1,'#',@akey2,'#',@akey3,'#',@akey4)"/>
      </xsl:attribute>
      <xsl:call-template name="name-attr">
	<xsl:with-param name="nmkey" select="@akey1"/>
      </xsl:call-template>
    </xsl:copy>
  </xsl:for-each>
</xsl:template>

<xsl:template name="sortbib-mk-keys">
  <xsl:param name="nodes"/>
  <xsl:for-each select="$nodes">
    <bibWrapper index="{position()}">
      <xsl:choose>
        <xsl:when test="*/t:author">
          <xsl:attribute name="akey1">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="*/t:author[1]"/>
            </xsl:call-template>
          </xsl:attribute>
          <xsl:attribute name="akey2">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="*/t:author[2]"/>
            </xsl:call-template>
          </xsl:attribute>
          <xsl:attribute name="akey3">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="*/t:author[3]"/>
            </xsl:call-template>
          </xsl:attribute>
          <xsl:attribute name="akey4">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="*/t:author[4]"/>
            </xsl:call-template>
          </xsl:attribute>
        </xsl:when>
        <xsl:when test="t:monogr/t:editor">
          <xsl:attribute name="akey1">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="t:monogr/t:editor[1]"/>
            </xsl:call-template>
          </xsl:attribute>
          <xsl:attribute name="akey2">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="t:monogr/t:editor[2]"/>
            </xsl:call-template>
          </xsl:attribute>
          <xsl:attribute name="akey3">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="t:monogr/t:editor[3]"/>
            </xsl:call-template>
          </xsl:attribute>
          <xsl:attribute name="akey4">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="t:monogr/t:editor[4]"/>
            </xsl:call-template>
          </xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
          <xsl:attribute name="akey1">
            <xsl:call-template name="map-name">
              <xsl:with-param name="nm" select="t:monogr/t:publisher[1]"/>
            </xsl:call-template>
          </xsl:attribute>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:attribute name="dkey">
        <xsl:call-template name="sortbib-mk-dkey">
          <xsl:with-param name="date" select="t:monogr/t:imprint/t:date[1]"/>
        </xsl:call-template>
      </xsl:attribute>
    </bibWrapper>
  </xsl:for-each>
</xsl:template>

<xsl:template name="sortbib-mk-dkey">
  <xsl:param name="date"/>
  <xsl:choose>
    <xsl:when test="not(translate($date,'0123456789',''))">
      <xsl:value-of select="$date"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="ndate"
  	          select="translate($date,
      '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-.;:,?! ',
      '0123456789')"/>
      <!-- 
          we're going to get NaN errors from the numeric compare 
          of bad dates if we carry on, so we might as well trap bad 
          dates early and bomb
       -->
      <xsl:if test="translate($ndate,'0123456789','')">
        <xsl:message terminate="yes"
         >Illegal characters in date '<xsl:value-of select="$date"
         />'</xsl:message>
      </xsl:if>
      <xsl:choose>
        <xsl:when test="string-length($ndate) = 0">
          <xsl:text>100000</xsl:text> <!-- n.d., forthcoming etc. -->
        </xsl:when>
        <xsl:otherwise>
         <xsl:value-of select="$ndate"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
