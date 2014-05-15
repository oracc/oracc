<xsl:transform 
   version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   xmlns:xcl="http://oracc.org/ns/xcl/1.0"
   xmlns:xtf="http://oracc.org/ns/xtf/1.0"
   xmlns="http://oracc.org/ns/xtf/1.0"
   >

<xsl:param name="lemm-mode" select="true()"/>
<xsl:include href="gdl-OATF.xsl"/>

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:template match="xtf:score">
  <xsl:apply-templates select="xtf:protocols/xtf:protocol"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:protocol[@type='project']">
  <xsl:value-of select="concat('#project: ', text(), '&#xa;')"/>
</xsl:template>

<xsl:template match="xtf:protocol[@type='atf']">
  <xsl:value-of select="concat('#atf: ', text(), '&#xa;')"/>
</xsl:template>

<xsl:template match="xtf:protocol[@type='link']">
  <xsl:value-of select="concat('#link: ', text(), '&#xa;')"/>
</xsl:template>

<xsl:template match="xtf:h"/>

<xsl:template match="xtf:lg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:l">
  <xsl:value-of select="concat('&amp;',ancestor::xtf:score/@project,':',ancestor::xtf:score/@xml:id,'&#x9;',@label,'&#xa;')"/>
</xsl:template>

<xsl:template match="xtf:div">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:c">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:v">
  <xsl:value-of select="concat(@varnum, ':&#x9;')"/>
  <xsl:apply-templates/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="xtf:cmt|xtf:nonx"/>

<xsl:template match="*"/>

</xsl:transform>
