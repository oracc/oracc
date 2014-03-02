<?xml version="1.0" encoding="utf-8"?>

<xsl:transform version="1.0" 
	       xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:xpd="http://oracc.org/ns/xpd/1.0"
	       xmlns:param="http://oracc.org/ns/esp-param/1.0"
	       xmlns:esp="http://oracc.org/ns/esp/1.0"
	       xmlns="http://www.w3.org/1999/xhtml"
	       >

<xsl:template match="/">
  <param:parameters>
    <param:title><xsl:value-of select="//xpd:name"/></param:title>
    <param:publisher>The <xsl:value-of select="//xpd:abbrev"/> Project</param:publisher>
    <param:host>http://oracc.org</param:host>
    <param:root></param:root>
    <param:common-headers>
      <meta name="DC.publisher" content="The {//xpd:abbrev} Project"/>
      <meta name="DC.language" scheme="ISO 639-2/T" content="eng"/>
      <meta name="DC.rights.copyright" content="CC BY-SA The {//xpd:abbrev} Project, 2014-"/>
      <meta name="DC.type" scheme="DCTERMS.DCMIType" content="Text" />
      <meta name="DC.format" content="text/html"/>
      <meta name="robots" content="noindex,nofollow"/>
    </param:common-headers>
    <param:dc-id-prefix><xsl:value-of select="concat(translate(/*/@n,'/','-'),'-oracc-org/page/')"/></param:dc-id-prefix>
    <param:main-menu-caption>Main menu</param:main-menu-caption>
    <param:footer><a href="http://creativecommons.org/licenses/by-sa/3.0/">CC BY-SA</a> The <xsl:value-of select="//xpd:abbrev"/> Project, 2014-</param:footer>
  </param:parameters>
</xsl:template>

</xsl:transform>
