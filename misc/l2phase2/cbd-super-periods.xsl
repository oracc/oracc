<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns:c="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:s="http://oracc.org/ns/sortkey/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:ex="http://exslt.org/common"
  extension-element-prefixes="ex"
  exclude-result-prefixes="c g s xff">

<xsl:template match="c:base/c:periods">
  <xsl:variable name="pnode" select="."/>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="document('')/*/*[@name='periods']/*">
      <cbd:group n="{@name}">
	<xsl:for-each select="*">
	  <xsl:copy-of select="$pnode/*[text()=current()/text()]"/>
	</xsl:for-each>
      </cbd:group>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template name="periods">
  <period name="Archaic">
    <p>Proto-Cuneiform</p>
    <p>ED I</p>
  </period>
  <period name="Early Dynastic">
    <p>ED IIIa</p>
    <p>ED IIIb</p>
  </period>
  <period name="Old Akkadian">
    <p>Old Akkadian</p>
  </period>
  <period name="Lagash II">
    <p>Lagash II</p>
  </period>
  <period name="Ur III">
    <p>Ur III</p>
  </period>
  <period name="Old Babylonian">
    <p>Early Old Babylonian</p>
    <p>Old Babylonian</p>
  </period>
  <period name="Post-OB">
    <p>Kassite</p>
    <p>Neo-Assyrian</p>
    <p>Neo-Babylonian</p>
    <p>Hellenistic</p>
  </period>
  <period name="Unknown">
    <p>Uncertain</p>
    <p>Unknown</p>
    <p>Unassigned</p>
  </period>
</xsl:template>

</xsl:stylesheet>
