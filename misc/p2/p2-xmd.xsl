<?xml version='1.0' encoding="utf-8"?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:md="http://oracc.org/ns/xmd/1.0"
  xmlns:xpd="http://oracc.org/ns/xpd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="md xpd">

<xsl:include href="html-standard.xsl"/>
<xsl:include href="p2-xmd-div.xsl"/>
<xsl:include href="p2-xtl-div.xsl"/>

<xsl:output method="xml" indent="yes" omit-xml-declaration="yes"/>

<xsl:param name="project" select="''"/>
<xsl:param name="text-base" select="''"/>

<xsl:template match="/md:xmd">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="webtype" select="'p2-p1'"/>
    <xsl:with-param name="p2" select="'yes'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <xsl:apply-templates mode="xmd" select="."/>
  <xsl:if test="/*/md:cat/md:id_composite">
    <xsl:apply-templates select="document(concat(/*/md:cat/md:id_composite,'.xtl'),/)"/>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>