<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
  xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:esp="http://oracc.org/ns/esp/1.0"
  xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
  xmlns:param="http://oracc.org/ns/esp-param/1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  version="2.0"
  xpath-default-namespace="http://www.w3.org/1999/xhtml"
  >
<xsl:output
    method="xml"
    encoding="utf-8"
    indent="no"
    />
<xsl:output
    name="html"
    method="html"
    encoding="utf-8"
    include-content-type="no"
    indent="no"
    media-type="text/html"
    omit-xml-declaration="yes"
    /><!-- for flash pages -->

<xsl:include href="esp2-functions.xslt"/>

<xsl:param name="oracc"/>
<xsl:param name="project"/>
<xsl:param name="projesp"/>
<xsl:param name="scripts"/>

<xsl:param name="output-file"/>
<xsl:param name="output-directory"/><!-- this is used for generating flash pages -->

<xsl:variable name="parameters" select="document ( concat($projesp, '/00web/00config/parameters.xml') )/param:parameters"/>


<xsl:variable name="last-modified-times" select="document ( concat($projesp, '/01tmp/last-modified-times.xml') )/esp:last-modified-times"/>

<xsl:variable name="index-page" select="//esp:index-list[1]/ancestor::struct:page[1]"/>
<xsl:variable name="glossary-page" select="//esp:glossary-list[1]/ancestor::struct:page[1]"/>
<xsl:variable name="techterms-page" select="//esp:techterms-list[1]/ancestor::struct:page[1]"/>
<xsl:variable name="site-map-page" select="//esp:site-map[1]/ancestor::struct:page[1]"/>
<xsl:variable name="privacy-page" select="//esp:privacy-policy[1]/ancestor::struct:page[1]"/>

<xsl:template match="/">
<!--  <xsl:message>Phase 4: Process low-level mark-up (= mark-up giving rise to other mark-up to be output as is), and add structural page components to head</xsl:message> -->
  <xsl:result-document href="{$output-file}">
    <xsl:apply-templates/>
  </xsl:result-document>
</xsl:template>

<!-- add structure to <head> -->
<xsl:template match="head">
  <xsl:variable name="relpath">
    <xsl:call-template name="set-relpath"/>
  </xsl:variable>
  <xsl:copy>
    <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
    <!-- title -->
    <title> 
      <xsl:value-of select="$parameters/param:title"/> 
      <xsl:if test="string-length($current-page/esp:title) > 0">
	- <xsl:value-of select="$current-page/esp:title"/>
      </xsl:if>
    </title>

    <!-- styles -->
<!--    <link rel="stylesheet" type="text/css" href="/css/oraccbase.css"/> -->
    <link rel="stylesheet" type="text/css" media="print" href="{$relpath}/css/print.css"/>
<!--    <link rel="stylesheet" type="text/css" media="handheld" href="{$relpath}/css/my-print.css"/> -->
    <link rel="stylesheet" type="text/css" media="handheld" href="{$relpath}/css/handheld.css"/>
<!--    <link rel="stylesheet" type="text/css" media="handheld" href="{$relpath}/css/my-handheld.css"/> -->
    <link rel="stylesheet" type="text/css" media="screen,projection" href="{$relpath}/css/screen.css"/>
    <!-- why were all the my-xxx.css files commented out? (because they don't seem to be used much [sjt]) -->
    <link rel="stylesheet" type="text/css" media="screen,projection" href="{$relpath}/css/my-screen.css"/>
    <link rel="stylesheet" type="text/css" media="browsers_that_ignore_the_media_type_attribute" href="{$relpath}/css/browsersthatsuck.css"/>

    <xsl:choose>
      <xsl:when test="string($parameters/param:cuneify/@default) = 'na'">
	<link rel="stylesheet" type="text/css" media="screen,print" href="/css/cuneify-ob.css" title="oldbabylonian"/>
	<link rel="stylesheet" type="text/css" media="screen,print" href="/css/cuneify-na.css" title="neoassyrian"/>
      </xsl:when>
      <xsl:when test="string($parameters/param:cuneify/@default) = 'ob'">
	<link rel="stylesheet" type="text/css" media="screen,print" href="/css/cuneify-na.css" title="neoassyrian"/>
	<link rel="stylesheet" type="text/css" media="screen,print" href="/css/cuneify-ob.css" title="oldbabylonian"/>
      </xsl:when>
      <xsl:otherwise>
	<link rel="stylesheet" type="text/css" media="screen,print" href="/css/cuneify-ob.css" title="oldbabylonian"/>
	<link rel="stylesheet" type="text/css" media="screen,print" href="/css/cuneify-na.css" title="neoassyrian"/>
      </xsl:otherwise>
    </xsl:choose>
    <!-- javascript -->
    <script type="text/javascript"><esp:comment>
	var sRoot = '<xsl:value-of select="$parameters/param:root"/>';
    // </esp:comment></script>
    <script type="text/javascript" src="/js/library.js"/>
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
    <xsl:apply-templates/>
    <!-- add Google Analytics block -->
<script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-32878242-1']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>
  </xsl:copy>
</xsl:template>

<!-- process bookmarks -->
<xsl:template match="esp:bookmark">
  <a id="{@id}" name="{@id}">
    <xsl:choose>
	<xsl:when test="@hide-highlight = 'yes'">
	  <xsl:apply-templates/>
	</xsl:when>
	<xsl:otherwise>
	  <span id="highlight_{@id}">
	    <xsl:apply-templates/>
	  </span>
	</xsl:otherwise>
    </xsl:choose>
  </a>
</xsl:template>

<!-- process accesskey listings -->
<xsl:template match="esp:accesskeys-listing">
  <table id="AccessKeys">
    <tr>
	<th>Key</th>
	<th>Page</th>
    </tr>
    <xsl:variable name="accesskey-pages">
	<struct:page accesskey="S"><esp:name>Skip to main content (on current page)</esp:name></struct:page>
	<struct:page accesskey="4"><esp:name>Search site (move to search box on current page)</esp:name></struct:page>
	<struct:page accesskey="8"><esp:name>Decrease text size</esp:name></struct:page>
	<struct:page accesskey="9"><esp:name>Increase text size</esp:name></struct:page>
	<xsl:for-each select="//struct:page[string ( @accesskey )]">
	  <xsl:copy>
	    <xsl:copy-of select="@* | esp:name"/>
	  </xsl:copy>
	</xsl:for-each>
    </xsl:variable>
    <xsl:for-each select="$accesskey-pages/struct:page">
	<xsl:sort select="@accesskey"/>
	<xsl:variable name="odd-even">
	  <xsl:choose>
	    <xsl:when test="position () mod 2 = 1">odd</xsl:when>
	    <xsl:otherwise>even</xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<tr class="{$odd-even}">
	  <td><b><xsl:value-of select="@accesskey"/></b></td>
	  <td><xsl:value-of select="esp:name"/></td>
	</tr>
    </xsl:for-each>
  </table>
</xsl:template>

<xsl:template name="maybe-local-url">
  <xsl:param name="root" select="$parameters/param:root"/>
  <xsl:param name="url" select="@url"/>
  <xsl:choose>
    <xsl:when test="starts-with($url,'http://') or @type='link'">
      <xsl:value-of select="$url"/>
    </xsl:when>
    <xsl:when test="starts-with($url,'/') and @type='link'">
      <xsl:value-of select="$url"/>
    </xsl:when>
    <xsl:when test="@type='link'">
      <xsl:value-of select="$root"/><xsl:value-of select="concat('/',$url)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="steps-down" select="count(ancestor::struct:page)"/>
      <!-- 
	   <xsl:message>Page <xsl:value-of select="ancestor-or-self::struct:page[1]/@id"/>
	   is <xsl:value-of select="$steps-down"/> steps down in struct</xsl:message> 
      -->
      <xsl:variable name="relpath">
	<xsl:choose>
	  <xsl:when test="@nesting">
	    <xsl:choose>
	      <xsl:when test="not(@nesting) or @nesting=0">.</xsl:when>
	      <!-- FIXME? do all these need an additional ../ prepended to them? -->
	      <xsl:when test="@nesting=1">..</xsl:when>
	      <xsl:when test="@nesting=2">../..</xsl:when>
	      <xsl:when test="@nesting=3">../../..</xsl:when>
	      <xsl:when test="@nesting=4">../../../..</xsl:when>
	      <xsl:when test="@nesting=5">../../../../..</xsl:when>
	      <xsl:when test="@nesting=6">../../../../../..</xsl:when>
	      <xsl:when test="@nesting=7">../../../../../../..</xsl:when>
	      <xsl:when test="@nesting=8">../../../../../../../..</xsl:when>
	      <xsl:when test="@nesting=9">../../../../../../../../..</xsl:when>
	      <xsl:otherwise>
		<xsl:message>esp2-phase-04.xslt: menu items nested greater than 9 deep (nesting=<xsl:value-of select="@nesting"/>)!</xsl:message>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:choose>
	      <xsl:when test="$steps-down=1">.</xsl:when>
	      <xsl:when test="$steps-down=2">..</xsl:when>
	      <xsl:when test="$steps-down=3">../..</xsl:when>
	      <xsl:when test="$steps-down=4">../../..</xsl:when>
	      <xsl:when test="$steps-down=5">../../../..</xsl:when>
	      <xsl:when test="$steps-down=6">../../../../..</xsl:when>
	      <xsl:when test="$steps-down=7">../../../../../..</xsl:when>
	      <xsl:when test="$steps-down=8">../../../../../../..</xsl:when>
	      <xsl:when test="$steps-down=9">../../../../../../../..</xsl:when>
	      <xsl:otherwise>
		<xsl:message>esp2-phase-04.xslt: page nested greater than 9 deep (nesting=<xsl:value-of select="$steps-down"/>)!</xsl:message>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:variable>
      <xsl:value-of select="$relpath"/><xsl:value-of select="concat($url,'index.html')"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- process internal links -->
<xsl:template match="esp:link[string ( @bookmark ) or string ( @page )] | esp:area[string ( @bookmark ) or string ( @page )]">
  <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
  <xsl:variable name="tag-has-content" select="count ( node () )"/>
  <xsl:variable name="bookmark" select="@bookmark"/>
  <xsl:variable name="id" select="if ( @page and not ( @page = $current-page/@id ) ) then @page else ''"/>
  <xsl:variable name="linked-page" select="//struct:page[@id = $id]"/> <!--FIXME: use xsl:key-->
  <xsl:variable name="linked-page-URL">
    <xsl:if test="$id">
	<xsl:if test="not ( $linked-page )">
	  <xsl:message>	WARNING! Broken internal link: no page is defined with id '<xsl:value-of select="$id"/>'</xsl:message>
	</xsl:if>

      <xsl:call-template name="maybe-local-url">
	<xsl:with-param name="url" select="$linked-page/@url"/>
      </xsl:call-template>

    </xsl:if>
  </xsl:variable>
  <xsl:variable name="anchor-name">
    <xsl:if test="@bookmark">
	<xsl:text>#</xsl:text><xsl:value-of select="@bookmark"/>
    </xsl:if>
  </xsl:variable>
  <xsl:variable name="link-title">
    <xsl:choose>
	<xsl:when test="@title">
	  <xsl:value-of select="@title"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:choose>
	    <xsl:when test="$id and (
			    $tag-has-content or
			    $linked-page/esp:title != $linked-page/esp:name or
			    $bookmark
			    )"
		      >
	      <xsl:if test="$bookmark">
		<xsl:text>Jump to </xsl:text>
		<xsl:value-of select="$linked-page//esp:bookmark/@title [../@id = $bookmark]"/>
		<xsl:text> in </xsl:text>
	      </xsl:if>
	      <xsl:value-of select="$linked-page/esp:title"/>
	    </xsl:when>
	    <xsl:when test="not ( $id )">
	      <xsl:text>Jump to </xsl:text>
	      <xsl:value-of select="$current-page//esp:bookmark/@title [../@id = $bookmark]"/>
	      <xsl:text> on this page</xsl:text>
	    </xsl:when>
	  </xsl:choose>
	</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="accesskey">
    <xsl:choose>
	<xsl:when test="string ( @accesskey )">
	  <xsl:value-of select="@accesskey"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="$linked-page/@accesskey"/>
	</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="onclick">
    <xsl:if test="not ( $id )">
	<xsl:text>if ( window.highlight ) window.setTimeout ( 'highlight ()', 100 )</xsl:text>
    </xsl:if>
  </xsl:variable>
  <xsl:if test="self::esp:link">
    <a href="{$linked-page-URL}{$anchor-name}">
	<xsl:if test="string ( $link-title )">
	  <xsl:attribute name="title" select="$link-title"/>
	</xsl:if>
	<xsl:if test="string ( $accesskey )">
	  <xsl:attribute name="accesskey" select="$accesskey"/>
	</xsl:if>
	<xsl:if test="string ( $onclick )">
	  <xsl:attribute name="onclick" select="$onclick"/>
	  <xsl:attribute name="onkeypress" select="$onclick"/>
	</xsl:if>
	<xsl:if test="@name">
	  <xsl:attribute name="name" select="@name"/>
	  <xsl:attribute name="id" select="@name"/>
	</xsl:if>
	<xsl:copy-of select="@class"/>
	<xsl:choose>
	  <xsl:when test="$tag-has-content">
	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:choose>
	      <xsl:when test="$id">
		<xsl:value-of select="$linked-page/esp:name"/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="$current-page//esp:bookmark/@title [../@id = $bookmark]"/>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:otherwise>
	</xsl:choose>
    </a>
  </xsl:if>
  <xsl:if test="self::esp:area">
    <area href="{$linked-page-URL}{$anchor-name}" alt="{@description}" shape="{@shape}" coords="{@coords}">
	<xsl:if test="string ( $link-title )">
	  <xsl:attribute name="title" select="$link-title"/>
	</xsl:if>
	<xsl:if test="string ( $accesskey )">
	  <xsl:attribute name="accesskey" select="$accesskey"/>
	</xsl:if>
	<xsl:if test="string ( $onclick )">
	  <xsl:attribute name="onclick" select="$onclick"/>
	  <xsl:attribute name="onkeypress" select="$onclick"/>
	</xsl:if>
	<xsl:if test="@name">
	  <xsl:attribute name="name" select="@name"/>
	  <xsl:attribute name="id" select="@name"/>
	</xsl:if>
    </area>
  </xsl:if>
</xsl:template>

<xsl:template name="twitter">
  <xsl:variable name="tag-has-content" select="count ( * | text() )"/>
  <xsl:variable name="processed-url" 
		select="if ( substring ( @url, 1, 1 ) = '~' ) then concat ( '.', substring ( @url, 2 ) ) else @url"/>
  <a href="{$processed-url}" class="external" target="_blank">
    <xsl:copy-of select="@class|@data-dnt|@data-widget-id"/>
    <xsl:if test="string ( @accesskey )">
      <xsl:attribute name="accesskey" select="@accesskey"/>
    </xsl:if>
    <xsl:variable name="link-title">
      <xsl:if test="substring ( @url, 1, 7 ) = 'http://'">
	<xsl:text>Link opens in new window</xsl:text>
      </xsl:if>
      <xsl:if test="substring ( @url, 1, 7 ) = 'http://' and ( string ( @title ) or ( $tag-has-content and string ( @site-name ) ) )">
	<xsl:text>: </xsl:text>
      </xsl:if>
      <xsl:choose>
	<xsl:when test="string ( @title )">
	  <xsl:value-of select="@title"/>
	</xsl:when>
	<xsl:when test="$tag-has-content and string ( @site-name )">
	  <xsl:value-of select="@site-name"/>
	</xsl:when>
      </xsl:choose>
    </xsl:variable>
    <xsl:attribute name="title" select="$link-title"/>
    <xsl:copy-of select="script"/>
  </a>
</xsl:template>

<!-- process external links (RH replaced text 'External site' with 'Link opens in new window')-->
<xsl:template match="esp:link[string ( @url )] | esp:area[string ( @url )]">
  <xsl:choose>
    <xsl:when test="@class='twitter-timeline'">
      <xsl:call-template name="twitter"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="tag-has-content" select="count ( * | text() )"/>
      <xsl:variable name="processed-url" select="if ( substring ( @url, 1, 1 ) = '~' ) then concat ( '.', substring ( @url, 2 ) ) else @url"/>
      <xsl:if test="self::esp:link">
	<a href="{$processed-url}" class="external" target="_blank">
	  <xsl:if test="string ( @accesskey )">
	    <xsl:attribute name="accesskey" select="@accesskey"/>
	  </xsl:if>
	  <xsl:variable name="link-title">
	    <xsl:if test="substring ( @url, 1, 7 ) = 'http://'">
	      <xsl:text>Link opens in new window</xsl:text>
	    </xsl:if>
	    <xsl:if test="substring ( @url, 1, 7 ) = 'http://' and ( string ( @title ) or ( $tag-has-content and string ( @site-name ) ) )">
	      <xsl:text>: </xsl:text>
	    </xsl:if>
	    <xsl:choose>
	      <xsl:when test="string ( @title )">
		<xsl:value-of select="@title"/>
	      </xsl:when>
	      <xsl:when test="$tag-has-content and string ( @site-name )">
		<xsl:value-of select="@site-name"/>
	      </xsl:when>
	    </xsl:choose>
	  </xsl:variable>
	  <xsl:if test="starts-with($link-title, 'Link ')">
	    <xsl:attribute name="target" select="'_blank'"/>
	  </xsl:if>
	  <xsl:attribute name="title" select="$link-title"/>
	  <xsl:choose>
	    <xsl:when test="$tag-has-content">
	      <xsl:apply-templates/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:choose>
		<xsl:when test="string ( @site-name )">
		  <xsl:value-of select="@site-name"/>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:value-of select="@url"/>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:otherwise>
	  </xsl:choose>
	</a>
	<xsl:if test="not ( @hide-print = 'yes' )">
	  <span class="externallinktext"> [<xsl:value-of select="@url"/>]</span>
	</xsl:if>
      </xsl:if>
      <xsl:if test="self::esp:area">
	<area href="{$processed-url}" alt="{@description}" shape="{@shape}" coords="{@coords}">
	  <xsl:if test="string ( @accesskey )">
	    <xsl:attribute name="accesskey" select="@accesskey"/>
	  </xsl:if>
	  <xsl:variable name="link-title">
	    <xsl:text>Link opens in new window</xsl:text>
	    <xsl:choose>
	      <xsl:when test="string ( @title )">
		<xsl:text>: </xsl:text><xsl:value-of select="@title"/>
	      </xsl:when>
	      <xsl:when test="$tag-has-content and string ( @site-name )">
		<xsl:text>: </xsl:text><xsl:value-of select="@site-name"/>
	      </xsl:when>
	    </xsl:choose>
	  </xsl:variable>
	  <xsl:attribute name="title" select="$link-title"/>
	</area>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- process page names and title -->
<xsl:template match="struct:page-name">
  <xsl:value-of select="//esp:name[ancestor::struct:page[1]/@id = current()/@page]"/>
</xsl:template>
<xsl:template match="struct:page-title">
  <xsl:value-of select="//esp:title[ancestor::struct:page[1]/@id = current()/@page]"/>
</xsl:template>

<!-- process headed paragraphs -->
<xsl:template match="esp:section">
  <xsl:if test="esp:shead/node ()">
    <div class="sectionheader">
	<xsl:apply-templates select="esp:shead/node ()"/>
    </div>
  </xsl:if>
  <div class="sectionbody">
    <xsl:apply-templates select="esp:sbody/node ()"/>
  </div>
</xsl:template>

<!-- process obfuscated info - such as email addresses -->
<xsl:template match="esp:deobfuscate">
  <script type="text/javascript"><esp:comment>
    document.writeln ( deobfuscate ( '<xsl:copy-of select="node ()"/>' ) );
  // </esp:comment></script>
</xsl:template>

<!-- process content modification date -->
<xsl:template match="esp:content-last-modified">
  <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
  <xsl:value-of select="$last-modified-times/esp:lmt[@file = $current-page/@file]"/>
</xsl:template>

<!-- process flash resources -->
<xsl:template match="esp:flash-movie">
  <xsl:message>The 'flash-movie' feature is no longer part of ESP.  Please revise your portal pages appropriately.</xsl:message>
</xsl:template>

<!-- copy the rest unchanged -->
<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
