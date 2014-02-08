<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:g="http://oracc.org/ns/gdl/1.0" 
	       xmlns:sl="http://oracc.org/ns/sl/1.0" xmlns="http://oracc.org/ns/sl/1.0">

<xsl:output method="xml" encoding="utf-8"/>

<xsl:variable name="ogsl" select="document('file:///usr/local/oracc/xml/ogsl/ogsl-sl.xml')/*"/>

<xsl:template match="sl:repertoire">
  <signlist>
    <xsl:apply-templates/>
  </signlist>
</xsl:template>

<xsl:template match="sl:sign">
  <xsl:copy>
    <xsl:copy-of select="@n|@icount"/>
    <xsl:attribute name="xml:id">
      <xsl:choose>
	<xsl:when test="string-length(@ref) > 0">
	  <xsl:value-of select="@ref"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="generate-id(.)"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:attribute>
    <xsl:variable name="ref" select="@ref"/>
    <xsl:for-each select="$ogsl">
      <xsl:variable name="ogsl-sign" select="id($ref)"/>
      <xsl:copy-of select="$ogsl-sign/sl:name"/>
      <xsl:copy-of select="$ogsl-sign/sl:sort"/>
      <xsl:copy-of select="$ogsl-sign/sl:list"/>
    </xsl:for-each>
    <utf8 hex="{@hex}" utf8="{@utf8}"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="sl:vals">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="sl:v">
  <v icount="{@icount}" ipct="{@ipct}"><xsl:apply-templates/></v>
</xsl:template>

</xsl:transform>
