<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       version="1.0">

<xsl:key name="oids" match="r" use="sign"/>

<xsl:template match="/*|/*/*">
  <xsl:copy>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="w">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:variable name="s" select="text()"/>
    <xsl:variable name="oid">
      <xsl:for-each select="document('w-oid.xml')">
	<xsl:value-of select="key('oids',$s)/oid"/>
      </xsl:for-each>
    </xsl:variable>
    <xsl:attribute name="oid">
      <xsl:value-of select="$oid"/>
    </xsl:attribute>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
