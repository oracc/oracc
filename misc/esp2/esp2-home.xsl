<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    >

<xsl:template match="/">
  <esp:page
      xmlns:esp="http://oracc.org/ns/esp/1.0"
      xmlns="http://www.w3.org/1999/xhtml"
      >

    <esp:name>Home</esp:name>
    <esp:title><xsl:value-of select="//xpd:name"/></esp:title>

    <html><head></head><body>

    <esp:image file="{//xpd:image}" description="{//xpd:image-alt}"/>

    <p><xsl:for-each select="//xpd:blurb"><xsl:apply-templates/></xsl:for-each></p>

    <p>You can view the project's content <esp:link url="/{/@n}/pager">here</esp:link>.</p>

    </body></html>
  </esp:page>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
