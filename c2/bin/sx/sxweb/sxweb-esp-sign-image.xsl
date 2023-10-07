<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    version="1.0">

<xsl:variable name="base" select="'../../../pctc'"/>
  
<xsl:template name="esp-sign-image">
  <xsl:variable name="ref" select="@ref"/>
  <xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
  <esp:image class="middle" height="30px" file="{$base}/{$header/@path}/{@loc}"
	     description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
</xsl:template>

</xsl:transform>

