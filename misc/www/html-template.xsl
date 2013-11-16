<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="dc xl">

<xsl:include href="html-util.xsl"/>

<!--
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
 -->

<xsl:output method="xml" encoding="utf-8"
   indent="yes"/>

<xsl:template name="make-html">
  <xsl:param name="webtype" select="'oraccpager'"/>
  <xsl:param name="with-hr" select="false()"/>
  <xsl:param name="with-trailer" select="false()"/>
  <xsl:param name="title"/>
  <html xmlns="http://www.w3.org/1999/xhtml">
    <head>
      <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
      <title>
        <xsl:value-of select="$title"/>
      </title>
      <link rel="shortcut icon" type="image/ico" href="/favicon.ico" />
      <link rel="stylesheet" type="text/css" href="/css/{$webtype}.css"/>
      <link rel="stylesheet" type="text/css" href="/{$project}/p3.css"/>
      <script src="/js/{$webtype}.js" type="text/javascript">
        <xsl:text> </xsl:text>
      </script>
    </head>
    <body>      
      <xsl:call-template name="call-back"/>
      <xsl:if test="$with-hr">
  	<hr class="trailer"/>
      </xsl:if>
      <xsl:if test="$with-trailer">
      <div class="trailer">
        <p class="qn">
          <a href="http://psd.museum.upenn.edu/">
	    <img class="topimg" src="/epsd/epsd.png" alt="ePSD icon"/></a>
<!--	  <xsl:text> Copyright (c) University of Pennsylvania Museum </xsl:text>
  	  <xsl:text> </xsl:text>
          <a href="http://psd.museum.upenn.edu/" 
	     title="electronic Pennsylvania Sumerian Dictionary">ePSD</a> 
	  contacts: <a href="mailto:stinney@sas.upenn.edu">Steve 
          Tinney</a> and <a href="mailto:phjones@sas.upenn.edu">Philip Jones</a>.
          <a href="http://validator.w3.org/check?uri=referer"><img
                 src="http://www.w3.org/Icons/valid-xhtml10" alt="Valid XHTML 1.0!" 
                 height="21" width="60" /></a>
          <a href="http://jigsaw.w3.org/css-validator/check/referer"><img
                 src="http://jigsaw.w3.org/css-validator/images/vcss" 
                 style="border:0;width:60px;height:21px"
                 alt="Valid CSS!" />
          </a>
-->
        </p>
      </div>
      </xsl:if>
    </body>
  </html>
</xsl:template>

</xsl:stylesheet>
