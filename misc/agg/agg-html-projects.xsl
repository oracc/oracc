<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    >
<xsl:output method="xml" indent="yes" encoding="utf-8"/>
<xsl:include href="url.xsl"/>

<xsl:template match="/">
  <html>
    <head>
      <link rel="icon" href="/favicon.ico" type="image/x-icon" />
      <link rel="shortcut icon" href="/favicon.ico" type="image/x-icon" />
      <link rel="stylesheet" type="text/css" href="/feoracchome.css"/>
      <title>Oracc Project List</title>
    </head>
    <body class="projlist">
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
	<img class="project-float" width="88px" height="66px"
	     src="{$img}"
	     alt="{xpd:image-alt}"/>
      </a>
    </p>
    <p class="proj-blurb">
      <xsl:choose>
	<xsl:when test="xpd:blurb/xpd:p">
	  <xsl:value-of select="xpd:blurb/xpd:p[1]"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="xpd:blurb"/>
	</xsl:otherwise>
      </xsl:choose>
    </p>
  </div>
</xsl:template>

</xsl:stylesheet>
