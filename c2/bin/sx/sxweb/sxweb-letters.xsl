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

<xsl:include href="mcol.xsl"/>
<xsl:include href="sxweb-esp-sign-image.xsl"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="sl:signlist">
  <xsl:apply-templates select="sl:letter"/> <!--[not(@num='1')]-->
  <ex:document href="{'signlist/00web/numbers.xml'}">
    <esp:page
	xmlns="http://www.w3.org/1999/xhtml"
	>
      <esp:name><xsl:value-of select="'Numbers'"/></esp:name>
      <esp:title><xsl:value-of select="'Numbers'"/></esp:title>
      <html>
	<head/>
	<body>
	  <xsl:call-template name="mcol">
	    <xsl:with-param name="columns" select="'4'"/>
	    <xsl:with-param name="nodes" select="sl:letter[@num='1']"/>
	    <xsl:with-param name="class" select="'pretty'"/>
	    <xsl:with-param name="tag" select="'letter'"/>
	  </xsl:call-template>
	</body>
      </html>
    </esp:page>
  </ex:document>
</xsl:template>

<xsl:template match="sl:letter"> <!-- [not(@num='1')] -->
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
	  <xsl:call-template name="mcol">
	    <xsl:with-param name="columns" select="'4'"/>
	    <xsl:with-param name="nodes" select="sl:sign"/>
	    <xsl:with-param name="class" select="'pretty'"/>
	  </xsl:call-template>
<!--
	  <xsl:for-each select="sl:sign">
	    <p>
	      <esp:link page="{@xml:id}">
		<strong><xsl:value-of select="translate(@n,'|','')"/></strong>
		<xsl:if test="count(sl:v) > 0">
		  <xsl:text>: </xsl:text>
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
		</xsl:if>
	      </esp:link>
	    </p>
	    </xsl:for-each>
-->
	</body>
      </html>
    </esp:page>
  </ex:document>
</xsl:template>

<xsl:template mode="mcol" match="sl:letter">
  <xsl:variable name="tdval">
    <xsl:for-each select="sl:sign[1]">
      <xsl:choose>
	<xsl:when test=".//g:n[1]/g:s[@form]">
	  <xsl:value-of select=".//g:n[1]/g:s/@form"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select=".//g:n/g:s[1]"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:variable>
  <td>
    <esp:link page="{@xml:id}">
      <xsl:value-of select="$tdval"/><br/>
      <xsl:for-each select="sl:sign[1]/sl:images/sl:i[@loc][1]">
	<xsl:call-template name="esp-sign-image">
	  <xsl:with-param name="height" select="'40px'"/>
	</xsl:call-template>
      </xsl:for-each>
    </esp:link>
  </td>
</xsl:template>

<xsl:template mode="mcol" match="sl:sign">
  <xsl:message>sl:sign <xsl:value-of select="@n"/></xsl:message>
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

<xsl:template match="text()"/>

</xsl:transform>
