<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:include href="formdiv.xsl"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="sl:letter">
  <ex:document href="{concat('signlist/00web/',@xml:id,'.xml')}"
    method="xml" encoding="utf-8"
    indent="yes">
    <esp:page>
      <esp:name><xsl:value-of select="@title"/></esp:name>
      <esp:title><xsl:value-of select="@title"/></esp:title>
      <esp:url><xsl:value-of select="@xml:id"/></esp:url>
      <html>
	<head/>
	<body>
<!--
	  <xsl:call-template name="form-div">
	    <xsl:with-param name="caller" select="'esp'"/>
	  </xsl:call-template>
 -->
	  <xsl:for-each select=".//sl:sign">
	    <p>
	      <esp:link page="{@xml:id}">
		<strong><xsl:value-of select="translate(@n,'|','')"/>: </strong>
		<xsl:for-each select="sl:v">
		  <xsl:choose>
		    <xsl:when test="@deprecated='yes'">
		      <span class="v-drop"><xsl:value-of select="@n"/></span>
		    </xsl:when>
		    <xsl:when test="@uncertain='yes'">
		      <span class="v-query"><xsl:value-of select="@n"/></span>
		    </xsl:when>
		    <xsl:otherwise>
		      <span class="v-ok"><xsl:value-of select="@n"/></span>
		    </xsl:otherwise>
		  </xsl:choose>
		  <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
		</xsl:for-each>
		<xsl:text>.</xsl:text>
	      </esp:link>
	    </p>
	  </xsl:for-each>
	</body>
      </html>
    </esp:page>
  </ex:document>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
