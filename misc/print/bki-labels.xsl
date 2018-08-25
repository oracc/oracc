<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:x="http://oracc.org/ns/xis/1.0"
		>

<xsl:key name="kv" match="t" use="@k"/>

<xsl:template match="c:sense">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:variable name="stem" select="@stem"/>
    <xsl:attribute name="stemkey">
      <xsl:for-each select="document('stems.xml',/)">
	<xsl:value-of select="key('kv',$stem)/@v"/>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>    
</xsl:template>

<xsl:template match="x:rr">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:variable name="value" select="substring-after(@value,':')"/>
    <xsl:attribute name="name">
      <xsl:for-each select="document('names.xml',/)">
	<xsl:value-of select="key('kv',$value)/@v"/>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>    
</xsl:template>

<!--
<xsl:template match="x:r">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:variable name="" select="@stem"/>
    <xsl:attribute name="stemkey">
      <xsl:for-each select="document('stems.xml',/)">
	<xsl:value-of select="key('kv',$stem)/@v"/>
      </xsl:for-each>
    </xsl:attribute>
    <xsl:apply-templates/>
  </xsl:copy>    
  </xsl:template>
  -->

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
