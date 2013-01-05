<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
    xmlns:tei="http://www.tei-c.org/ns/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="xml" encoding="utf-8"/>

<xsl:template match="/">
  <listBibl xmlns="http://www.tei-c.org/ns/1.0">
    <xsl:apply-templates/>
  </listBibl>
</xsl:template>

<xsl:template match="tei:biblStruct|biblStruct">
  <xsl:apply-templates mode="ns" select="."/>
</xsl:template>

<xsl:template mode="ns" match="*">
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*[not(local-name()='id')]"/>
    <xsl:apply-templates mode="ns"/>
  </xsl:element>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>