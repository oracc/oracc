<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:gdl="http://oracc.org/ns/gdl/1.0">

<xsl:output method="text" encoding="utf8"/>

<xsl:key name="utf8s" match="*[@gdl:utf8]" use="@gdl:utf8"/>

<xsl:template match="*[@gdl:utf8]">
  <xsl:if test="generate-id(.) = generate-id(key('utf8s',@gdl:utf8))">
    <xsl:choose>
      <xsl:when test="string-length(@gdl:utf8)=1">
	<xsl:value-of select="@gdl:utf8"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="splitchars">
	  <xsl:with-param name="chars" select="@gdl:utf8"/>
	</xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template name="splitchars">
  <xsl:param name="chars"/>
  <xsl:choose>
    <xsl:when test="string-length($chars)>0">
      <xsl:value-of select="substring($chars,1,1)"/>
      <xsl:text>&#xa;</xsl:text>
      <xsl:call-template name="splitchars">
	<xsl:with-param name="chars" select="substring($chars,2)"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise/>
  </xsl:choose>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>