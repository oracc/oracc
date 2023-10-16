<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       version="1.0">

<xsl:key name="oids" match="w" use="@oid"/>

<xsl:template match="*[@xml:id]">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:variable name="oid" select="@xml:id"/>
    <xsl:variable name="icount">
      <xsl:for-each select="document('sign-instances.xml',/)">
	<xsl:value-of select="key('oids',$oid)/@n"/>
      </xsl:for-each>
    </xsl:variable>
    <xsl:if test="string-length($icount)>0">
      <xsl:attribute name="icount">
	<xsl:value-of select="$icount"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="text()">
  <xsl:copy-of select="."/>
</xsl:template>

</xsl:transform>
