<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns:c="http://oracc.org/ns/cbd/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:s="http://oracc.org/ns/sortkey/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:ex="http://exslt.org/common"
  extension-element-prefixes="ex"
  exclude-result-prefixes="c g s xff">

<xsl:output method="xml" encoding="utf-8"/>
<xsl:template match="/">
  <entries>
    <xsl:apply-templates/>
  </entries>
</xsl:template>

<xsl:template match="c:entry">
  <feature>
    <properties>
      <project><xsl:value-of select="/*/@project"/></project>
      <lang><xsl:value-of select="/*/@xml:lang"/></lang>
      <headword><xsl:value-of select="concat(c:cf,'[',c:gw,']',c:pos)"/></headword>
      <id><xsl:value-of select="@xml:id"/></id>
      <instances><xsl:value-of select="@xis"/></instances>
    </properties>
    <geometry>
      <type>Point</type>
      <coordinates><xsl:value-of select="c:pl_coord"/></coordinates>
    </geometry>
  </feature>
</xsl:template>

</xsl:stylesheet>
