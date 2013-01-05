<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    version="1.0">

<xsl:import href="html-standard.xsl"/>
<xsl:import href="gdl-HTML.xsl"/>

<xsl:param name="basename"/>
<xsl:param name="project"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:variable name="lower" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="upper" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>
<xsl:variable name="jsArticleBegin">javascript:cbdArticleFromContent('</xsl:variable>
<xsl:variable name="jsResultsBegin">javascript:cbdResultsFromOutline('</xsl:variable>
<xsl:variable name="jsEnd">')</xsl:variable>

<xsl:template match="/">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>    
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="sl:signlist">
  <xsl:apply-templates/>  
</xsl:template>

<xsl:template match="sl:sign|sl:form">
  <div class="{local-name(.)}">
    <div class="info">
      <h2>
	<xsl:value-of select="@n"/>
	<xsl:if test="@var">
	  <xsl:value-of select="concat(' (=',ancestor::sl:sign/@n,@var,')')"/>
	</xsl:if>
	<xsl:if test="string-length(sl:uname/text()) > 0">
	  <xsl:text>: </xsl:text>
	  <xsl:value-of select="sl:uname"/>
	</xsl:if>
      </h2>
      <xsl:if test="sl:list">
	<p>
	  <xsl:for-each select="sl:list">
	    <xsl:value-of select="concat('(',@n,')')"/>
	    <xsl:if test="not(position()=last())">
	      <xsl:text>; </xsl:text>
	    </xsl:if>
	  </xsl:for-each>
	</p>
      </xsl:if>
    </div>
    <div class="values">
      <p>
	<span class="values-heading">Values:</span>
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
	</xsl:for-each>
      </p>
    </div>
    <xsl:if test="sl:form">
      <div class="forms">
	<xsl:apply-templates select="sl:form"/>
      </div>
    </xsl:if>
  </div>
</xsl:template>

</xsl:transform>