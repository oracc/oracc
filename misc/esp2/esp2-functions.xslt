<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
	version="2.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:esp="http://oracc.org/ns/esp/1.0"
	xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
	xmlns="http://www.w3.org/1999/xhtml"
	xpath-default-namespace="http://www.w3.org/1999/xhtml"
>

<xsl:function name="esp:make-alphanumeric">
  <xsl:param name="in-string"/>
  <xsl:variable name="out-string">
    <xsl:analyze-string select="lower-case ( string ( $in-string ) )" regex="[a-z0-9]+">
      <xsl:matching-substring>
	<xsl:value-of select="."/>
      </xsl:matching-substring>
    </xsl:analyze-string>
  </xsl:variable>
  <xsl:value-of select="string ( $out-string )"/>
</xsl:function>
	
<xsl:template name="set-relpath">
  <xsl:variable name="nest-level" select="count(ancestor::struct:page)"/>
  <xsl:choose>
    <xsl:when test="$nest-level=1">.</xsl:when>
    <xsl:when test="$nest-level=2">..</xsl:when>
    <xsl:when test="$nest-level=3">../..</xsl:when>
    <xsl:when test="$nest-level=4">../../..</xsl:when>
    <xsl:when test="$nest-level=5">../../../..</xsl:when>
    <xsl:when test="$nest-level=6">../../../../..</xsl:when>
    <xsl:when test="$nest-level=7">../../../../../..</xsl:when>
    <xsl:when test="$nest-level=8">../../../../../../..</xsl:when>
    <xsl:when test="$nest-level=9">../../../../../../../..</xsl:when>
    <xsl:otherwise>
      <xsl:message>set-relpath: pages nested greater than 9 deep!</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
