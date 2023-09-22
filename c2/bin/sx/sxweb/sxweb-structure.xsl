<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    exclude-result-prefixes="sl dc xh"
    version="1.0">

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:template match="/">
  <struct:page file="home.xml" id="home" type="page">
    <xi:include href="00lib/signlist-structure-top.xml">
      <xi:fallback/>
    </xi:include>
    <xsl:for-each select="*/sl:letter">
      <struct:page file="{@xml:id}.xml" id="{@xml:id}" type="page">
	<xsl:for-each select=".//sl:sign">
	  <struct:page file="{@xml:id}.xml" id="{@xml:id}" type="page" hide-menu-link="yes"/>
	</xsl:for-each>
      </struct:page>
    </xsl:for-each>
    <xi:include href="00lib/signlist-structure-bot.xml">
      <xi:fallback/>
    </xi:include>
  </struct:page>
</xsl:template>

</xsl:transform>
