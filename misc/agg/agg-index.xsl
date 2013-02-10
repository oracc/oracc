<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    >
<xsl:output method="xml" indent="yes" encoding="utf-8" omit-xml-declaration="yes"/>
<xsl:include href="url.xsl"/>

<xsl:template match="/">
  <html>
    <head>
      <title>Oracc: The Open Richly Annotated Cuneiform Corpus</title>
      <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
      <!--  <meta http-equiv="Refresh" content="850" /> -->
      <link rel="icon" href="/favicon.ico" type="image/x-icon" />
      <link rel="shortcut icon" href="/favicon.ico" type="image/x-icon" />
      <link rel="stylesheet" type="text/css" href="oracchome.css" />
      <link rel="alternate" type="application/atom+xml" title="Oracc News - Atom" href="http://oracc.blogspot.com/feeds/posts/default" />
      <link rel="alternate" type="application/rss+xml" title="Oracc News - RSS" href="http://oracc.blogspot.com/feeds/posts/default?alt=rss" />
      <script src="/oracchome.js" type="text/javascript">var x;
</script>
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
    <body class="projlist">

      <div class="buttons">
	<div class="form">
	  <form method="get" action="http://oracc.museum.upenn.edu/find" id="form1">
	    <p class="buttonbar"> <!-- tabindex="1" etc; accesskey="0" etc -->
	      <a href="/readme.html"><span class="button">README</span></a
	      ><a href="webservices.html"><span class="button">WEB SERVICES</span></a
	      ><a href="/doc"><span class="button">DOCUMENTATION</span></a
	      ><a href="javascript:void()" 
	      onclick="document.forms['form1'].submit(); return false;"><span class="button">SEARCH ORACC</span></a
	      ><input class="textbox" name="phrase" id="phrase" type="text" size="14" accesskey="s" value=""
	      /></p>
	  </form>
	</div>
	<div class="social">
	  <p>
	    <a href="http://www.facebook.com/opencuneiform" title="Oracc Facebook Page"
	       class="facebook"><img width="19" height="19" src="/img/f_logo.png"/></a
	       ><a href="http://oracc.blogspot.com" title="Oracc Blog"
	       class="blogspot"><img width="19" height="19" src="/img/blogspot-icon.png"/></a
	       ><a href="http://www.twitter.com/opencuneiform" title="Oracc Twitter Account (@opencuneiform)"
	       class="blogspot"><img width="22" height="22" src="/img/twitter-bird-light-bgs.png"/></a
	       ><a href="http://creativecommons.org/licenses/by-sa/3.0/"><img height="20" alt="Creative Commons License"
	       src="http://i.creativecommons.org/l/by-sa/3.0/88x31.png"/></a
	       ></p>
	</div>
      </div>

      <div class="banner-container">
	<div class="voracc">
	  <p class="oracc o1">O</p>
	  <p class="oracc o2">r</p>
	  <p class="oracc o3">a</p>
	  <p class="oracc o4">c</p>
	  <p class="oracc o5">c</p>
	</div>
	<div class="banner">
	  <p>
	    <span class="small"><span class="blet lite">T</span>
	    <span class="blet lite">H</span>
	    <span class="blet lite">E</span></span><br />
	    <span class="blet cap">O</span>
	    <span class="blet">P</span>
	    <span class="blet">E</span>
	    <span class="blet">N</span><br />
	    <span class="blet cap">R</span>
	    <span class="blet">I</span>
	    <span class="blet">C</span>
	    <span class="blet">H</span>
	    <span class="blet">L</span>
	    <span class="blet">Y</span><br />
	    <span class="clet cap">A</span>
	    <span class="clet">N</span>
	    <span class="clet">N</span>
	    <span class="clet">O</span>
	    <span class="clet">T</span>
	    <span class="clet">A</span>
	    <span class="clet">T</span>
	    <span class="clet">E</span>
	    <span class="clet">D</span><br />
	    <span class="blet cap">C</span>
	    <span class="blet">U</span>
	    <span class="blet">N</span>
	    <span class="blet">E</span>
	    <span class="blet">I</span>
	    <span class="blet">F</span>
	    <span class="blet">O</span>
	    <span class="blet">R</span>
	    <span class="blet">M</span><br />
	    <span class="blet cap">C</span>
	    <span class="blet">O</span>
	    <span class="blet">R</span>
	    <span class="blet">P</span>
	    <span class="blet">U</span>
	  <span class="blet">S</span></p>
	</div>
      </div>

      <div class="blurb">
	<p class="just">Oracc is a collaborative effort to develop a complete corpus of
	cuneiform whose rich annotation and open licensing support the next
	generation of scholarly research.  Created by Steve Tinney, Oracc is
	steered by Eleanor Robson, Tinney, and Niek Veldhuis.</p>
      </div>

      <div class="projects">
	<xsl:apply-templates/>
      </div>
    </body>
  </html>
</xsl:template>

<xsl:template match="projects">
  <xsl:for-each select="*[./xpd:public='yes' and not(./xpd:option[@name='project-hide']/@value='yes')]">
    <xsl:sort select="@n"/>
    <xsl:apply-templates select="."/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xpd:project">
  <xsl:variable name="img">
    <xsl:value-of select="concat('/agg/', @n, '.png')"/>
  </xsl:variable>
  <xsl:variable name="url">
    <xsl:call-template name="url-name"/>
  </xsl:variable>
  <xsl:variable name="proj-class">
    <xsl:choose>
      <xsl:when test="contains(@n, '/')">
	<xsl:text>subproject-entry</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>project-entry</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <div class="{$proj-class}">
    <h2 class="proj-head">
      <a target="_blank" href="{$url}">
	<xsl:choose>
	  <xsl:when test="xpd:abbrev = xpd:name">
	    <xsl:value-of select="xpd:abbrev"/>
	  </xsl:when>
	  <xsl:when test="starts-with(xpd:name, xpd:abbrev)">
	    <xsl:value-of select="xpd:name"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="xpd:abbrev"/>
	    <xsl:text>: </xsl:text>
	    <xsl:value-of select="xpd:name"/>
	  </xsl:otherwise>
	</xsl:choose>
      </a>
    </h2>
    <p class="proj-img">
      <a target="_blank" href="{$url}">
	<!--  width="88px" height="66px" : not necessary now we thumbnail these -->
	<img class="project-float"
	     src="{$img}"
	     alt="{xpd:image-alt}"/>
      </a>
    </p>
    <p class="proj-blurb">
      <xsl:choose>
	<xsl:when test="xpd:blurb/xpd:p">
	  <xsl:apply-templates mode="xhtml" select="xpd:blurb/xpd:p[1]"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:apply-templates mode="xhtml" select="xpd:blurb"/>
	</xsl:otherwise>
      </xsl:choose>
    </p>
  </div>
</xsl:template>


<xsl:template mode="xhtml" match="xpd:blurb|xpd:p">
  <xsl:apply-templates mode="xhtml"/>
</xsl:template>

<xsl:template mode="xhtml" match="*">
  <xsl:element name="{local-name(.)}">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="xhtml"/>
  </xsl:element>
</xsl:template>

<xsl:template mode="xhtml" match="text()">
  <xsl:value-of select="."/>
</xsl:template>

</xsl:stylesheet>
