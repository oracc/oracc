<?xml version='1.0' encoding="utf-8"?>
<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xh="http://www.w3.org/1999/xhtml">
<xsl:param name="block"/>
<xsl:param name="project"/>

<xsl:template match="/">
  <html>
    <head>
      <link rel="stylesheet" type="text/css" href="/css/oraccscreen.css" media="screen" />
      <link rel="stylesheet" type="text/css" href="/css/oraccprint.css" media="print" />
      <link rel="stylesheet" type="text/css" href="/{$project}/project.css" media="screen" />
      <link rel="stylesheet" type="text/css" href="/css/p2-p1.css" media="screen" />
      <link rel="stylesheet" type="text/css" href="/{$project}/p2.css" media="screen" />
      <script src="/js/p2.js" type="text/javascript"> </script>
      <script src="/js/p2-shortcuts.js" type="text/javascript"> </script>
      <script src="/js/p2-keys.js" type="text/javascript"> </script>      
      <title>
	<xsl:value-of select="translate(id($block)/*[1]/@label,'ḫ','h')"/>
      </title>
    </head>
    <body>
      <h1 class="h2"><xsl:value-of select="id($block)/*[1]/@label"/></h1>
      <!--<xsl:copy-of select="id($block)"/>-->
      <xsl:apply-templates select="id($block)"/>
    </body>
  </html>
</xsl:template>

<xsl:template match="xh:a">
  <xsl:if test="not(contains(@href, 'pop1sig'))">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:if>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
