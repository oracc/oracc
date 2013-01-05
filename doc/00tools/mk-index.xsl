<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns="http://www.w3.org/1999/xhtml" 
		xmlns:d="http://oracc.org/ns/xdf/1.0"
		xmlns:dc="http://purl.org/dc/elements/1.1"
		xmlns:dcterms="http://purl.org/dc/terms/"
		xmlns:h="http://www.w3.org/1999/xhtml" 
   		xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
		xmlns:xi="http://www.w3.org/2001/XInclude" 
		version="1.0"
		exclude-result-prefixes="d dc dcterms xsi">

<xsl:param name="doco-type"/>
<xsl:param name="ns" select="false()"/>

<xsl:include href="dirname.xsl"/>

<xsl:template match="xdf">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
<title>Oracc <xsl:value-of select="$doco-type"/> Documentation</title>
<link rel="stylesheet" type="text/css" href="/css/oraccdoc.css"/>
<script src="/js/oraccdoc.js" type="text/javascript"><xsl:text>
</xsl:text></script>
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
<body class="subpage">
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

<div class="subpage">
<h1 class="title">Oracc <xsl:value-of select="$doco-type"/> Documentation</h1>
<xsl:apply-templates/>
</div>
</body>
</html>
</xsl:template>

<xsl:template match="dl">
  <dl>
    <xsl:apply-templates/>
  </dl>
</xsl:template>

<xsl:template match="index-h">
  <h2 class="index-h">
    <xsl:apply-templates/>
  </h2>
</xsl:template>

<xsl:template match="index-sub">
  <h3 class="index-sub">
    <a>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </a>
  </h3>
</xsl:template>

<xsl:template match="d:doc">
  <xsl:call-template name="dt"/>
</xsl:template>

<xsl:template match="d:secondary">
  <xsl:variable name="base" select="substring-before(
				      substring-after(@xml:base,'/ns/'),
				      '/')"/>
  <xsl:variable name="version" select="substring-before(
				         substring-after(@xml:base,concat($base,'/')),
					 '/')"/>
  <xsl:variable name="href">
    <xsl:value-of select="concat('/ns/',$base,'/',$version,'/',@xml:id,'.html')"/>
  </xsl:variable>
  <xsl:call-template name="dt">
    <xsl:with-param name="href" select="$href"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="dt">
  <xsl:param name="href"/>
  <dt>
    <xsl:choose>
      <xsl:when test="$ns">
	<a href="{concat(substring-before(d:meta[1]/dcterms:identifier,'.org/'),
		         '.org/',
			 substring-after(d:meta[1]/dcterms:identifier,'.org/'))}">
	  <xsl:value-of select="d:meta[1]/dc:title"/>:
	  <xsl:value-of select="d:meta[1]/dcterms:alternative"/>
	</a>
      </xsl:when>
      <xsl:when test="string-length($href)>0">
	<a href="{$href}"> <!-- or $href if d:secondary -->
	  <xsl:value-of select="*/dc:title"/>
	</a>
      </xsl:when>
      <xsl:otherwise>
	<xsl:variable name="dirname">
	  <xsl:call-template name="dirname">
	    <xsl:with-param name="f" select="@xml:base"/>
	  </xsl:call-template>
	</xsl:variable>
	<a href="{$dirname}"> <!-- or generate web dir from xml:base -->
	  <xsl:value-of select="*/dc:title"/>
	</a>
      </xsl:otherwise>
    </xsl:choose>
  </dt>
  <dd><xsl:value-of select="*/dc:description"/></dd>
</xsl:template>

</xsl:stylesheet>
