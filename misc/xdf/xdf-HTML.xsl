<?xml version="1.0" encoding="utf-8"?>
<xsl:transform version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xdf="http://oracc.org/ns/xdf/1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:dc="http://purl.org/dc/elements/1.1"
  xmlns:dcterms="http://purl.org/dc/terms/"
  xmlns:xi="http://www.w3.org/2001/XInclude" 
  exclude-result-prefixes="xdf dc dcterms xh">

<!--
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
-->
<xsl:output method="xml" encoding="utf-8" indent="yes"/>

<xsl:param name="parent-title"/>
<xsl:param name="parent-href"/>
<xsl:param name="prefix"/>
<xsl:param name="rev-date"/>

<xsl:variable name="href_in"  select="' #:()[]+-,&#xa;'"/>
<xsl:variable name="href_out" select="'__________'"/>

<xsl:template match="xdf:doc">
  <html xml:lang="en">
  <xsl:call-template name="html-head"/>
  <body onload="onresize=setTOCoffset; setTOCoffset()">

<div class="docbuttons">
  <div class="form">
    <form method="get" action="http://oracc.museum.upenn.edu/doc/find" id="form1">
      <p class="buttonbar"> <!-- tabindex="1" etc; accesskey="0" etc -->
	<a href="/readme.html"><span class="button">README</span></a
	><a href="/"><span class="button">Oracc Home</span></a
	><a href="javascript:void()" 
	onclick="document.forms['form1'].submit(); return false;"><span class="button">SEARCH DOCUMENTATION</span></a
	><input class="textbox" name="phrase" id="phrase" type="text" size="14" accesskey="s" value=""
	/></p>
    </form>
  </div>
  <div class="docsocial">
    <p>
      <a href="http://www.facebook.com/opencuneiform" title="Oracc Facebook Page"
	 class="facebook"><img width="19" height="19" src="/img/f_logo.png"/></a
	 ><a href="http://oracc.blogspot.com" title="Oracc Blog"
	 class="blogspot"><img width="19" height="19" src="/img/blogspot-icon.png"/></a
	 ><a href="http://www.twitter.com/opencuneiform" title="Oracc Twitter Account (@opencuneiform)"
	 class="twitter"><img width="22" height="22" src="/img/twitter-bird-light-bgs.png"/></a
	 ><a href="http://creativecommons.org/licenses/by-sa/3.0/" class="cc-by-sa"><img height="20" alt="Creative Commons License"
	 src="http://i.creativecommons.org/l/by-sa/3.0/88x31.png"/></a
	 ></p>
  </div>
</div>

  <xsl:choose>
    <xsl:when test="not(@toc='none')">
      <div class="withtoc">
        <xsl:call-template name="make-toc"/>
        <div class="body">
          <xsl:apply-templates/>
          <xsl:call-template name="trailer"/>
        </div>
      </div>
    </xsl:when>
    <xsl:otherwise>
      <div class="sanstoc">
        <xsl:call-template name="div-header"/>
        <div class="body">
          <xsl:apply-templates/>
          <xsl:call-template name="trailer"/>
        </div>
      </div>
    </xsl:otherwise>
  </xsl:choose>
  </body>
  </html>
</xsl:template>

<xsl:template match="xdf:meta">
  <div class="doc-banner">
  <h1 class="title">
    <xsl:apply-templates select="dc:title"/>
    <xsl:if test="string-length(dcterms:alternative) > 0">
      <xsl:text>: </xsl:text>
      <xsl:apply-templates select="dcterms:alternative"/>
    </xsl:if>
  </h1>
  <xsl:if test="string-length(dcterms:identifier) > 0">
    <h2 class="ns">
      <xsl:text>(</xsl:text>
      <xsl:apply-templates select="dcterms:identifier"/>
      <xsl:text>)</xsl:text>
    </h2>
  </xsl:if>
  <h2 class="author">
    <xsl:apply-templates select="dc:creator"/>
    <br/>
    <xsl:value-of select="concat('Version of ', $rev-date)"/>
<!--
    <xsl:if test="string-length(dc:date) > 0">
      <br/>
      <xsl:apply-templates select="dc:date"/>
    </xsl:if>
 -->
  </h2>
  </div>
  <h1><a name="intro">Introduction</a></h1>
  <p>
    <xsl:apply-templates select="dc:description"/>
  </p>
</xsl:template>

<!-- drop nodes intended only for the secondary stream -->
<xsl:template match="xdf:secondary"/>

<xsl:template match="*[contains(@class,'secondary')]"/>

<xsl:template match="xh:h1|xh:h2|xh:h3|xh:h4">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
<!--    <a name="{generate-id(.)}"> -->
    <xsl:choose>
      <xsl:when test="@id">
	<xsl:apply-templates/>	
      </xsl:when>
      <xsl:otherwise>
	<a name="{translate(text(),$href_in,$href_out)}">
	  <xsl:apply-templates/>
	</a>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:div[contains(@class,'atf')]">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <a name="{generate-id(.)}"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xdf:schema">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xdf:rnc">
  <pre class="listing">
    <xsl:apply-templates/>
  </pre>
</xsl:template>

<xsl:template match="dc:*|dcterms:*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xdf:*">
  <xsl:message>xdf-HTML.xsl: unhandled XDF element <xsl:value-of 
       select="local-name()"/></xsl:message>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template name="make-toc">
  <div class="speedbar" id="speedbar">
    <xsl:if test="string-length($parent-href) > 0">
      <p class="toc h1">
	<a href="{$parent-href}"><xsl:value-of select="$parent-title"/></a>
      </p>
      <hr class="toc"/>
    </xsl:if>
    <p class="toc h1">
      <a href="#top"><xsl:text>Introduction</xsl:text></a>
    </p>
    <hr class="toc"/>
    <!--    <xsl:for-each select="//xh:h1|//xh:h2|//xh:h3|//xh:div[@class='atf']"> -->
    <xsl:for-each select="//xh:h1|//xh:h2|//xh:h3">
      <xsl:choose>
	<xsl:when test="self::xh:div">
	  <xsl:variable name="hnum">
	    <xsl:for-each select="(preceding::xh:h1
				  |preceding::xh:h2
				  |preceding::xh:h3)[last()]">
	      <xsl:value-of select="substring(local-name(),2)"/>
	    </xsl:for-each>
	  </xsl:variable>
	  <xsl:variable name="divh" select="concat('h',number($hnum)+1)"/>
	  <p class="toc {$divh}">
<!--	    <a href="{concat('#',generate-id(.))}"><xsl:text>ATF</xsl:text></a> -->
            <xsl:choose>
	      <xsl:when test="@id">
		<a href="{concat('#',@id)}"><xsl:text>ATF</xsl:text></a>
	      </xsl:when>
	      <xsl:otherwise>
		<a href="{concat('#',translate(.,$href_in,$href_out))}"><xsl:text>ATF</xsl:text></a>
	      </xsl:otherwise>
	    </xsl:choose>
	  </p>	    
	</xsl:when>
	<xsl:otherwise>
	  <xsl:if test="not(contains(@class,'secondary'))">
  	    <p class="toc {local-name()}">
<!--	      <a href="{concat('#',generate-id(.))}"><xsl:apply-templates/></a> -->
              <xsl:choose>
		<xsl:when test="@id">
		  <a href="{concat('#',@id)}"><xsl:apply-templates/></a>
		</xsl:when>
		<xsl:otherwise>
		  <a href="{concat('#',translate(.,$href_in,$href_out))}"><xsl:apply-templates/></a>
		</xsl:otherwise>
	      </xsl:choose>
	    </p>
	  </xsl:if>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:if test="not(contains(@class,'secondary')) and
		    (following::xh:h1|following::xh:h2|following::xh:h3
		     |following::xh:div[@class='atf'])
		    [1][local-name()='h1']">
        <hr class="toc"/>
      </xsl:if>
    </xsl:for-each>
  </div>
</xsl:template>

<xsl:template name="html-head">
  <xsl:variable name="title">
    <xsl:value-of select="xdf:meta/dc:title"/>
    <xsl:if test="string-length(xdf:meta/dcterms:alternative)">
      <xsl:text>: </xsl:text>
      <xsl:value-of select="xdf:meta/dcterms:alternative"/>
    </xsl:if>
  </xsl:variable>
<head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
  <title><xsl:value-of select="$title"/></title>
  <link rel="stylesheet" type="text/css" href="{$prefix}/css/oraccdoc.css"/>
<!--  <script src="/js/oraccdoc.js" type="text/javascript"><![CDATA[ ]]></script> -->
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
</head>
</xsl:template>

<xsl:template mode="print" match="dc:title">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template name="trailer">
<hr/>
<p class="qn">
<!--
<a href="http://validator.w3.org/check?uri=referer"><img
 src="http://www.w3.org/Icons/valid-xhtml10" alt="Valid XHTML 1.0!" 
height="31" width="88" /></a>
<a href="http://jigsaw.w3.org/css-validator/check/referer">
  <img style="border:0;width:88px;height:31px"
       src="http://jigsaw.w3.org/css-validator/images/vcss" 
       alt="Valid CSS!" />
 </a>
 -->
 Questions about this document may be directed to
the Oracc Steering Committee (osc at oracc dot org).</p>
</xsl:template>

</xsl:transform>
