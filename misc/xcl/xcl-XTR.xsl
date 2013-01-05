<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:template match="/">
  <xsl:value-of select="concat('&amp;',/*/@ref,' = ',/*/@n,'&#xa;',
    '@translation en&#xa;')"/>
  <xsl:for-each select="//xcl:c[@type='unit']">
    <xsl:value-of select="concat('&#xa;@unit ',substring-after(@xml:id,'.U'),'&#xa;')"/>
    <xsl:text>@span </xsl:text>
    <xsl:value-of select="xcl:d[@type='line-start'][1]/@label"/>
    <xsl:if test="count(xcl:d[@type='line-start']) > 1">
      <xsl:value-of select="concat(' - ', 
			    xcl:d[@type='line-start'][last()]/@label)"/>
    </xsl:if>
    <xsl:text>&#xa;&#xa;</xsl:text>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
