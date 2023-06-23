<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
	version="2.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:esp="http://oracc.org/ns/esp/1.0"
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
	
</xsl:stylesheet>
