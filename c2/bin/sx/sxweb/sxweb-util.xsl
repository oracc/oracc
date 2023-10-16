<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:g="http://oracc.org/ns/gdl/1.0"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:param name="base" select="'../../../pctc'"/>
  
<xsl:template name="esp-sign-image">
  <xsl:variable name="ref" select="@ref"/>
  <xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
  <esp:image class="middle" height="30px" file="{$base}/{$header/@path}/{@loc}"
	     description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
</xsl:template>

<xsl:template mode="mcol" match="sl:sign">
  <!--<xsl:message>sl:sign <xsl:value-of select="@n"/></xsl:message>-->
  <td class="name-sign">
    <esp:link page="{@xml:id}">
      <p class="sl-td-name"><xsl:value-of select="@n"/></p>
      <xsl:for-each select="(.//sl:images/sl:i[@loc])[1]">
	<p class="sl-td-sign">
	  <xsl:call-template name="esp-sign-image">
	    <xsl:with-param name="height" select="'40px'"/>
	  </xsl:call-template>
	</p>
      </xsl:for-each>
    </esp:link>
  </td>
</xsl:template>

</xsl:transform>
