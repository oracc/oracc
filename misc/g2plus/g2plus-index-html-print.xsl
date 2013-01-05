<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xh">

<xsl:output method="xml" encoding="utf-8" indent="no" omit-xml-declaration="yes"/>

<xsl:template match="/">
  <div class="index-section">
    <xsl:copy-of select="/*/xh:body/xh:div[@class='tocbanner']"/>
    <xsl:for-each select="/*/xh:body/xh:p[@class='ix']">
      <p class="ix">
        <xsl:copy-of select="xh:span[@class='what']"/>
	<xsl:for-each select="xh:span[@class='summary']">
  	  <span class="where">
	    <xsl:copy-of select="xh:a/xh:span[@class='ixcf']"/>
  	    <span class="gw">
 	      <xsl:text>[</xsl:text>
	      <xsl:for-each select="xh:a/xh:span[@class='gw']">
	        <xsl:copy-of select="*|text()"/>
	      </xsl:for-each>
 	      <xsl:text>]</xsl:text>
	    </span>
 	  </span>
	</xsl:for-each>
      </p>
    </xsl:for-each>
  </div>
</xsl:template>

</xsl:stylesheet>
