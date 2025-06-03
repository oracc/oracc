<?xml version="1.0" encoding="utf-8"?>

<!-- esp-phase-01.xsl operates on structure.xml  -->

<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns:param="http://oracc.org/ns/esp-param/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:h="http://www.w3.org/1999/xhtml"
    xmlns:ex="http://exslt.org/common"
    extension-element-prefixes="ex"
    exclude-result-prefixes="esp param struct"
    version="1.0" 
    >

<xsl:output  method="xml" encoding="utf-8" indent="no"/>

<xsl:include href="esp2-menu.xsl"/>

<!-- In the Oracc context relative paths are only necessary for
     navigating within the document tree; for all of the uses that ESP
     makes of set-relpath we can just use an absolute path starting
     with the project -->
<xsl:template name="set-relpath">
  <xsl:param name="project"/>
  <xsl:value-of select="concat('/',$project)"/>
</xsl:template>

<xsl:template name="esp2-banner-div">
  <xsl:param name="parameters"/>
  <xsl:param name="current-page"/>
  <xsl:param name="nomenu" select="false()"/>
  <xsl:param name="top-index-link" select="''"/>
  <!--<xsl:message>esp2-banner-div 1 slform=<xsl:value-of select="$parameters/param:slform"/></xsl:message>-->
  <div>
    <xsl:choose>
      <xsl:when test="count(ancestor::struct:page)=1 and $parameters/param:banner">
	<!--<xsl:message>esp2-banner-div 2</xsl:message>-->
	<xsl:attribute name="id"><xsl:text>Banner</xsl:text></xsl:attribute>
	<xsl:variable name="cnode" select="."/>
	<xsl:for-each select="$parameters/param:banner">
	  <xsl:apply-templates>
	    <xsl:with-param name="cnode" select="$cnode"/>
	  </xsl:apply-templates>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<!--<xsl:message>esp2-banner-div 3</xsl:message>-->
	<xsl:attribute name="id"><xsl:text>Header</xsl:text></xsl:attribute>

	<xsl:if test="not($nomenu) and $parameters/param:menu-dropdown='yes'">
	  <xsl:call-template name="insert-menu">
	    <xsl:with-param name="current-page" select="$current-page"/>
	  </xsl:call-template>
	</xsl:if>

	<xsl:choose>
	  <xsl:when test="$parameters/param:slform='yes'">
	    <div id="HeadTitleForm">
	      <div id="HeadTitle">
		<xsl:choose>
		  <xsl:when test="$current-page/ancestor::struct:page[1]">
		    <esp:link page="{/struct:page/@id}" nesting="{count($current-page/ancestor::struct:page)}">
		      <xsl:copy-of select="$parameters/param:title/node()"/>
		    </esp:link>
		  </xsl:when>
		  <xsl:when test="string-length($top-index-link)>0">
		    <a href="{$top-index-link}">
		      <xsl:copy-of select="$parameters/param:title/node()"/>
		    </a>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:copy-of select="$parameters/param:title/node()"/>
		  </xsl:otherwise>
		  </xsl:choose>
		<xsl:text>: </xsl:text>
	      </div>
	      <div id="HeadForm">
		<form name="sl" id="sl" action="javascript://" onsubmit="return slpage();">
		  <input type="text" size="20" id="k" name="k" value=""/>
		</form>
	      </div>
	    </div>
	  </xsl:when>
	  <xsl:otherwise>
	    <span id="HeadTitle">
	      <xsl:choose>
		<xsl:when test="$current-page/ancestor::struct:page[1]">
		  <esp:link page="{/struct:page/@id}" nesting="{count($current-page/ancestor::struct:page)}">
		    <xsl:copy-of select="$parameters/param:title/node()"/>
		  </esp:link>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:copy-of select="$parameters/param:title/node()"/>
		</xsl:otherwise>
	      </xsl:choose>
	    </span>
	    <xsl:if test="$parameters/param:subtitle">
	      <br/>
	      <span id="HeadSubtitle">
		<xsl:copy-of select="$parameters/param:subtitle/node()"/>
	      </span>
	    </xsl:if>
	  </xsl:otherwise>
	  </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
    </div>
</xsl:template>

<xsl:template name="esp2-head-content">
  <xsl:param name="parameters"/>
  <xsl:param name="project"/>
  <xsl:param name="glossary-page" select="false()"/>
  <xsl:param name="techterms-page" select="false()"/>
  <xsl:param name="index-page" select="false()"/>
  <xsl:param name="site-map-page" select="false()"/>
  <xsl:variable name="relpath">
    <xsl:call-template name="set-relpath">
      <xsl:with-param name="project" select="$project"/>
    </xsl:call-template>
  </xsl:variable>
    <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
    <!-- title -->
    <meta charset="UTF-8"/>
    <title> 
      <xsl:value-of select="$parameters/param:title"/> 
      <xsl:if test="string-length($current-page/esp:title) > 0">
	- <xsl:value-of select="$current-page/esp:title"/>
      </xsl:if>
    </title>

    <!-- Oracc system font support -->
    <link rel="stylesheet" type="text/css" media="screen,projection,print" href="/css/fonts.css"/>

    <!-- styles -->
    <link rel="stylesheet" type="text/css" media="print" href="{$relpath}/css/print.css"/>
    <link rel="stylesheet" type="text/css" media="screen,projection" href="{$relpath}/css/screen.css"/>
   
    <!--<xsl:message>esp2-phase-04 head child count = <xsl:value-of select="count(*)"/></xsl:message>-->
    <xsl:copy-of select="link[@rel='stylesheet']"/>
    
    <!-- javascript -->
    <xsl:if test="false()"> <!-- what was this script with a commented var for?? -->
    <script type="text/javascript"><esp:comment>
	var sRoot = '<xsl:value-of select="$parameters/param:root"/>';
    // </esp:comment></script>
    </xsl:if>
    <script type="text/javascript" src="/js/library.js">&#xa0;</script>
    <!-- ICRA tag (assert 'none of the above' in all categories) -->
    <meta http-equiv="pics-label" content='(pics-1.1 "http://www.icra.org/ratingsv02.html" l gen true for "{$parameters/param:host}{$relpath}" r (nz 1 vz 1 lz 1 oz 1 cz 1))'/>
    <!-- shortcut icon document relation -->
    <link rel="shortcut icon" href="{$relpath}/favicon.ico" type="image/x-icon"/>
    <!-- P3P policy document relation -->
    <link rel="P3Pv1" href="{$relpath}/w3c/p3p.xml"/>
    <!-- navigation document relations (screen only) -->
    <xsl:if test="$current-page/ancestor::struct:page[1]">
	<link rel="top" title="{/struct:page/esp:title}" href="{$relpath}{/struct:page/@url}"/>
    </xsl:if>
    <xsl:variable name="up" select="$current-page/ancestor::struct:page[1][not ( @hide-menu-link = 'yes' )][ancestor::struct:page[1]]"/>
    <xsl:if test="$up">
	<link rel="up" title="{$up/esp:title}" href="{$relpath}{$up/@url}"/>
    </xsl:if>
    <xsl:if test="not ( $current-page/@hide-menu-link = 'yes' )">
	<xsl:variable name="prev" select="$current-page/preceding-sibling::struct:page[not ( @hide-menu-link )][1]"/>
	<xsl:if test="$prev">
	  <link rel="prev" title="{$prev/esp:title}" href="{$relpath}{$prev/@url}"/>
	</xsl:if>
	<xsl:variable name="next" select="$current-page/following-sibling::struct:page[not ( @hide-menu-link )][1]"/>
	<xsl:if test="$next">
	  <link rel="next" title="{$next/esp:title}" href="{$relpath}{$next/@url}"/>
	</xsl:if>
    </xsl:if>
    <xsl:if test="$glossary-page and generate-id ( $glossary-page ) != generate-id ( $current-page )">
	<link rel="glossary" title="{$glossary-page/esp:title}" href="{$relpath}{$glossary-page/@url}"/>
    </xsl:if>
    <xsl:if test="$techterms-page and generate-id ( $techterms-page ) != generate-id ( $current-page )">
	<link rel="techterms" title="{$techterms-page/esp:title}" href="{$relpath}{$techterms-page/@url}"/>
    </xsl:if>
    <xsl:if test="$index-page and generate-id ( $index-page ) != generate-id ( $current-page )">
	<link rel="index" title="{$index-page/esp:title}" href="{$relpath}{$index-page/@url}"/>
    </xsl:if>
    <xsl:if test="$site-map-page and generate-id ( $site-map-page ) != generate-id ( $current-page )">
	<link rel="contents" title="{$site-map-page/esp:title}" href="{$relpath}{$site-map-page/@url}"/>
    </xsl:if>
    <!-- Yes, really, we emit a meta generator tag to make it easier to separate ESP html from non-ESP html -->
    <meta name="generator" content="Oracc ESP"/>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"/>
    <!-- Dublin Core metadata -->
    <link rel="schema.DC" href="http://purl.org/dc/elements/1.1/"/>
    <link rel="schema.DCTERMS" href="http://purl.org/dc/terms/"/>
    <meta name="DC.title" content="{$current-page/esp:title}"/>
    <xsl:if test="$current-page/esp:title != $current-page/esp:name">
	<meta name="DC.title.alternative" content="{$current-page/esp:name}"/>
    </xsl:if>
    <meta name="DC.identifier" scheme="DCTERMS.URI" content="{$parameters/param:host}{$relpath}{$current-page/@url}"/>
    <meta name="DC.identifier" content="{$parameters/param:dc-id-prefix}{$current-page/@id}"/>
    <!-- common headers -->
    <xsl:copy-of select="$parameters/param:common-headers/node ()"/>
    <!-- process rest of content (if any) -->
    <xsl:if test="local-name(..)='html'">
      <xsl:copy-of select="*"/>
    </xsl:if>
    <!-- add Google Analytics block -->
<!-- Google tag (gtag.js) -->
<script async="async" src="https://www.googletagmanager.com/gtag/js?id=G-0QKC3P5HJ1">&#xa0;</script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());

  gtag('config', 'G-0QKC3P5HJ1');
</script>
</xsl:template>

<xsl:template name="insert-menu">
  <xsl:param name="current-page"/>
  <!-- main menu (screen only) -->
  <div id="menu-button">&#x2261;</div>
  <div id="Menu" class="hidden">
    <xsl:if test="$parameters/param:menu-dropdown='yes'">
      <xsl:attribute name="class">dd</xsl:attribute>
    </xsl:if>
    <xsl:if test="$parameters/param:main-menu-caption">
      <div id="MenuCaption">
        <xsl:copy-of select="$parameters/param:main-menu-caption/node()"/>
      </div>
    </xsl:if>
    <!--<xsl:when test="$current-page/ancestor::struct:page[1]">
        <div id="SelfInMenu" class="only"><xsl:value-of select="esp:name"/></div>
	</xsl:when>
	<xsl:otherwise>
        <esp:link page="{$current-page/@id}" class="{$li-class}"/>
	</xsl:otherwise>-->
    <!--<xsl:variable name="home-page-only">
        <esp:dummy><xsl:for-each select="/"><xsl:copy/></xsl:for-each></esp:dummy>
	</xsl:variable>-->
    <xsl:call-template name="menu">
      <xsl:with-param name="menu-page" select="/struct:page"/>
      <xsl:with-param name="current-page" select="$current-page"/>
      <xsl:with-param name="first-link-page" select="/struct:page"/>
    </xsl:call-template>
  </div>
</xsl:template>

</xsl:stylesheet>
