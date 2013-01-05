<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns="http://oracc.org/ns/xpd/1.0"
    version="1.0">

<xsl:output method="xml" indent="yes" encoding="utf-8"/>
<xsl:strip-space elements="xpd:*"/>
<xsl:param name="inherit-from"/>

<xsl:template match="xpd:project">
  <project>
    <xsl:copy-of select="@n"/>
    <xsl:apply-templates/>
    <option name="l2" value="yes"/>
    <inherit project="{$inherit-from}"/>
  </project>
</xsl:template>

<xsl:template match="xpd:name|xpd:abbrev|xpd:project-type|xpd:public|xpd:blurb|xpd:image|xpd:image-alt|xpd:type|xpd:logo">
  <xsl:copy-of select="."/>
</xsl:template>

<xsl:template match="*"/>

</xsl:stylesheet>