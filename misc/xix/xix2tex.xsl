<xsl:stylesheet version="1.0"
  xmlns:xix="http://emegir.info/index"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:import href="generic-tex.xsl"/>
<xsl:output method="text"/>

<xsl:template match="xix:where">
  <xsl:call-template name="tex-array"/>
</xsl:template>

</xsl:stylesheet>
