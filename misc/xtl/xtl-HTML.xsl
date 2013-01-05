<?xml version='1.0' encoding="utf-8"?>
<xsl:stylesheet version="1.0" 
  xmlns:xtl="http://oracc.org/ns/list/1.0"
  xmlns:xmd="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:param name="block"/>
<xsl:template match="/">
  <html>
    <head>
      <link rel="stylesheet" type="text/css" href="/oracc/www/xo.css"/>
<!--      <link rel="stylesheet" type="text/css" href="/oracc/www/oracc.css"/> -->
<!--      <script src="/oracc/www/oracc.js" type="text/javascript">
	<xsl:text> </xsl:text>
      </script> -->
      <script src="/oracc/www/xo.js" type="text/javascript">
	<xsl:text> </xsl:text>
      </script>
      <title>
	<xsl:value-of select="concat('Texts list for ', translate(/*/@n,'ḫ','h'))"/>
      </title>
    </head>
    <body>
      <div class="backbar">
	<p><a onclick="backToOutline()">[BACK TO OUTLINE]</a></p>
      </div>
      <div class="xtlbody">
	<h1 class="xodisplay"><xsl:value-of select="/*/@n"/></h1>
	<xsl:apply-templates/>
      </div>
    </body>
  </html>
</xsl:template>

<xsl:template match="xtl:group">
  <div class="xootl" id="{@type}">
    <h3 class="xo"><xsl:value-of select="@type"/></h3>
    <div class="xol2">
      <xsl:apply-templates/>
    </div>
  </div>
</xsl:template>

<xsl:template match="xtl:item">
  <xsl:variable name="type">
    <xsl:choose>
      <xsl:when test="starts-with(@text,'P')">exemplar</xsl:when>
      <xsl:when test="starts-with(@text,'Q')">composite</xsl:when>
      <xsl:otherwise/>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="image-nodes" select="document(concat(@path,'/',@text,'.xmd'))/*/xmd:cat/xmd:images/*"/>
  <xsl:variable name="image" select="count($image-nodes)"/>
  <xsl:variable name="best-image">
    <xsl:choose>
      <xsl:when test="$image-nodes[@type='p']">
	<xsl:value-of select="$image-nodes[@type='p' and @scale='full']/@src"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$image-nodes[@type='l' and @scale='full']/@src"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="imgclass">
    <xsl:choose>
      <xsl:when test="$image > 0">withimg</xsl:when>
      <xsl:otherwise>sansimg</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <p class="xo {$imgclass}">
    <xsl:if test="$image > 0">
      <a target="_blank" href="http://psd.museum.upenn.edu/oracci/dl/{$best-image}" class="haveimg">[IMG]</a>
    </xsl:if>
    <a href="javascript:showexemplar('{@text}','{@text}.1','')">
      <xsl:value-of select="@n"/>
    </a>
  </p>
</xsl:template>

</xsl:stylesheet>
