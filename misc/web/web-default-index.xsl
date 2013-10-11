<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="xpd"
    >
<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>
<xsl:param name="HOST"/>
<xsl:template match="/">
  <html>
    <head>
      <meta charset="utf-8"/>
      <link rel="stylesheet" type="text/css" 
	    href="/css/wwwhome.css"/>
      <link rel="stylesheet" type="text/css" 
	    href="/{/*/@n}/p2.css"/>
      <link rel="stylesheet" type="/css/default-index.css"/>
      <title>
	<xsl:value-of select="/*/xpd:abbrev"/>
	<xsl:text> Home Page</xsl:text>
      </title>
    </head>
    <body class="subpage default-index">
      <h1>
	<span class="default-index-abbrev"><xsl:value-of select="/*/xpd:abbrev"/></span>
	<xsl:if test="not(translate(/*/xpd:abbrev,'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz')
		         =translate(/*/xpd:name,  'ABCDEFGHIJKLMNOPQRSTUVWXYZ ','abcdefghijklmnopqrstuvwxyz'))">
	  <br/>
	  <xsl:value-of select="/*/xpd:name"/>
	</xsl:if>
      </h1>
      <xsl:choose>
	<xsl:when test="/*/xpd:type='corpus'">
	  <p class="default-index-link"><a href="http://{$HOST}/{/*/@n}/corpus"
		>Browse the <xsl:value-of
	  select="/*/xpd:abbrev"/> corpus here</a>.</p>
	</xsl:when>
	<xsl:when test="/*/xpd:type='catalogue'">
	  <p class="default-index-link"><a href="http://{$HOST}/{/*/@n}/corpus"
		>Browse the <xsl:value-of
	  select="/*/xpd:abbrev"/> catalogue here</a>.</p>
	</xsl:when>
	<xsl:when test="/*/xpd:type='dictionary'">
	  <p class="default-index-link"
	     ><a href="http://{$HOST}/{/*/@n}/cbd/{/*/xpd:option[@name='pager-default-lang']/@value}"
		>Consult the <xsl:value-of
	  select="/*/xpd:abbrev"/></a>.</p>
	</xsl:when>
      </xsl:choose>
      <p class="default-index-img">
	<img height="400px" 
	     src="http://oracc.museum.upenn.edu/{/*/@n}/images/{/*/xpd:image}"
	     alt="{/*/xpd:image-alt}"/>
      </p>
      <xsl:choose>
	<xsl:when test="/*/xpd:blurb/xpd:p">
	  <xsl:for-each select="/*/xpd:blurb/xpd:p">
	    <p class="default-index-blurb"><xsl:apply-templates/></p>
	  </xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
	  <p class="default-index-blurb">
	    <xsl:apply-templates select="/*/xpd:blurb"/>
	  </p>
	</xsl:otherwise>
      </xsl:choose>
    </body>
  </html>
</xsl:template>

</xsl:stylesheet>
