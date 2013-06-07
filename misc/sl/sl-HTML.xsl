<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<xsl:import href="html-standard.xsl"/>
<xsl:import href="gdl-HTML.xsl"/>

<xsl:param name="basename"/>
<xsl:param name="project"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:variable name="lower" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="upper" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>
<xsl:variable name="jsArticleBegin">javascript:cbdArticleFromContent('</xsl:variable>
<xsl:variable name="jsResultsBegin">javascript:cbdResultsFromOutline('</xsl:variable>
<xsl:variable name="jsEnd">')</xsl:variable>

<xsl:template match="sl:signlist">
  <xsl:call-template name="make-html">
    <xsl:with-param name="title" select="'OGSL'"/>
    <xsl:with-param name="project" select="'ogsl'"/>
    <xsl:with-param name="webtype" select="'cbd'"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>
  </xsl:call-template>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="sl:sign">
  <ex:document href="{concat('02www/signs/',@xml:id,'.html')}"
    method="xml" encoding="utf-8"
    doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
    indent="yes">
    <xsl:call-template name="make-html">
      <xsl:with-param name="title" select="'OGSL'"/>
      <xsl:with-param name="project" select="'ogsl'"/>
      <xsl:with-param name="webtype" select="'cbd'"/>
      <xsl:with-param name="with-hr" select="false()"/>
      <xsl:with-param name="with-trailer" select="false()"/>
    </xsl:call-template>
  </ex:document>
</xsl:template>

<xsl:template match="sl:form">
  <xsl:call-template name="call-back"/>
</xsl:template>

<xsl:template name="call-back">
  <xsl:choose>
    <xsl:when test="self::sl:sign">
      <xsl:call-template name="form-div"/>
      <xsl:call-template name="sign-or-form"/>
    </xsl:when>
    <xsl:when test="self::sl:form">
      <xsl:call-template name="sign-or-form"/>
    </xsl:when>
    <xsl:when test="self::sl:signlist">
      <xsl:call-template name="form-div"/>
      <div class="ogsl-intro">
	<h1>Welcome to OGSL, the Oracc Global Sign List.</h1>
	<p>To get started, type a sign name or value in the box above and click on the dropdown.</p>
      </div>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>sl-HTML.xsl: unhandled tag <xsl:value-of select="local-name(.)"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sign-or-form">
  <div class="ogsl-{local-name(.)}">
    <div class="ogsl-info">
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
	    <xsl:value-of select="@n"/>
	    <xsl:if test="not(position()=last())">
	      <xsl:text>; </xsl:text>
	    </xsl:if>
	  </xsl:for-each>
	</p>
      </xsl:if>
    </div>
    <div class="ogsl-values">
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
      <div class="ogsl-signforms">
	<xsl:apply-templates select="sl:form"/>
      </div>
    </xsl:if>
  </div>
</xsl:template>

<xsl:template name="form-div">
  <div class="ogsl-form" id="ogsldiv">
    <form name="ogslform" id="ogslform" action="/cgi-bin/slse" method="post" target="_top">
      <input type="hidden" name="project" value="ogsl"/>
      <p class="unisearch">
	<input type="submit" value="FIND:" name="x"/>
	<input type="text" size="10" name="k1" value=""/>
	<select name="ext" onmouseup="submit()">
	  <option selected="yes" value="">SIGN</option>
	  <option value="forms">FORMS</option>
	  <option value="h">HOMOPHONES</option>
	  <option value="c">COMPOUNDS</option>
	  <option value="cinit">INITIAL</option>
	  <option value="clast">FINAL</option>
	  <option value="contains">TIMES</option>
	  <option value="contained">CONTAINED</option>
	</select>
      </p>
    </form>
  </div>
</xsl:template>

</xsl:transform>
