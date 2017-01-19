<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:a="http://relaxng.org/ns/compatibility/annotations/1.0" 
	       xmlns:d="http://oracc.org/ns/d" 
	       xmlns:m="http://oracc.org/ns/m" 
	       xmlns:xpd="http://oracc.org/ns/xpd/1.0" 
	       xmlns:s="http://relaxng.org/ns/structure/1.0" 
	       xmlns="http://relaxng.org/ns/structure/1.0" 
	       ns="http://oracc.org/ns/xpd/1.0" 
	       datatypeLibrary="http://www.w3.org/2001/XMLSchema-datatypes"
	       >
<xsl:output method="text"/>
<xsl:template match="s:element">
  <xsl:value-of select="concat(s:attribute[@name='name']/s:value,'&#xa;')"/>
</xsl:template>
<xsl:template match="text()"/>
</xsl:transform>
