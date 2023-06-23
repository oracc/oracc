<?xml version='1.0' encoding="utf-8"?>
<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:param name="block"/>
<xsl:param name="project"/>

<xsl:template match="/">
  <html>
    <head>
      <link rel="stylesheet" type="text/css" href="/css/oraccscreen.css"/>
      <link rel="stylesheet" type="text/css" href="/{$project}/pager.css"/>
      <script src="/oracc.js" type="text/javascript">
	<xsl:text> </xsl:text>
      </script>
      <script src="/js/oraccpager.js" type="text/javascript">
	<xsl:text> </xsl:text>
      </script>
      <title>
	<xsl:value-of select="translate(/*/@xlabel,'ḫ','h')"/>
      </title>
<!-- Google tag (gtag.js) -->
<script async="async" src="https://www.googletagmanager.com/gtag/js?id=G-0QKC3P5HJ1"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());

  gtag('config', 'G-0QKC3P5HJ1');
</script>
    </head>
    <body>
      <h1 class="h2"><xsl:value-of select="id($block)/*[1]/@label"/></h1>
      <xsl:copy-of select="id($block)"/>
    </body>
  </html>
</xsl:template>

</xsl:stylesheet>
