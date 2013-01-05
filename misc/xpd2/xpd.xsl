<xsl:stylesheet 
    xmlns:oracc="http://oracc.org/ns/oracc/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    >

<xsl:template name="xpd-option">
  <xsl:param name="config-xml"/>
  <xsl:param name="default" select="''"/>
  <xsl:param name="option"/>
  <xsl:variable name="opt-node" 
		select="document($config-xml)/xpd:project|/*/*/xpd:project"/>
  <xsl:choose>
    <xsl:when test="$opt-node/*[@name=$option]">
      <xsl:value-of select="$opt-node/*[@name=$option]/@value"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$default"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="xpd-option-node">
  <xsl:param name="config-xml"/>
  <xsl:param name="option"/>
  <xsl:variable name="opt-node" 
		select="document($config-xml)/xpd:project|/*/*/xpd:project"/>
  <xsl:copy-of select="$opt-node/*[@name=$option]"/>
</xsl:template>

<xsl:template name="xpd-label">
  <xsl:param name="config-xml"/>
  <xsl:param name="label-surface"/>
  <xsl:variable name="opt-node" 
		select="document($config-xml)/xpd:project/xpd:labels
			|/*/*/xpd:project/xpd:labels
			|/*/xpd:project/xpd:labels"/>
<!--  <xsl:message>opt-node = <xsl:value-of select="local-name($opt-node)"
  />; label-surface = <xsl:value-of select="$label-surface"/></xsl:message> -->
  <xsl:choose>
    <xsl:when test="$opt-node/*[@name=$label-surface]">
      <xsl:value-of select="$opt-node/*[@name=$label-surface]/@value"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$label-surface"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
