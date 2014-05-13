<xsl:transform 
   version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   xmlns:xcl="http://oracc.org/ns/xcl/1.0"
   xmlns:xtf="http://oracc.org/ns/xtf/1.0"
   xmlns="http://oracc.org/ns/xtf/1.0"
   >

<xsl:template match="xtf:score">
  <transliteration cols="1">
    <xsl:copy-of select="@xml:id|@n|@xml:lang|@project|@haslinks"/>
    <xsl:apply-templates/>
  </transliteration>
</xsl:template>

<xsl:template match="xtf:lg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:l">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:attribute name="score-block-ref">
      <xsl:value-of select="concat('sb.',/*/@xml:id,'.',1+count(../preceding-sibling::xtf:lg))"/>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xtf:c">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:v"/>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>
