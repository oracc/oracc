<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:ex="http://exslt.org/common"
	       extension-element-prefixes="ex"
	       version="1.0">

<xsl:template match="e">
  <ex:document href="{concat('01bld/tislists/',w/@oid,'.tis')}"
	       method="text" encoding="utf-8">
    <xsl:apply-templates mode="print" select="l"/>
  </ex:document>  
</xsl:template>

<xsl:template mode="print" match="l">
  <xsl:value-of select="text()"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="text()"/>

</xsl:transform>
