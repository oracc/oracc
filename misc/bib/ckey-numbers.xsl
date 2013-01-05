<xsl:stylesheet version="1.0" 
  xmlns:exslt="http://exslt.org/common"
  xmlns:bib="http://oracc.org/ns/bib/1.0"
  extension-element-prefixes="exslt"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="reflist">
  <citekeys>
  <xsl:for-each select="*">
    <ckey bib:id="{@bib:id}">
      <names/>
      <abbrev><xsl:value-of select="position()"/></abbrev>
    </ckey>
  </xsl:for-each>
  </citekeys>    
</xsl:template>

</xsl:stylesheet>
