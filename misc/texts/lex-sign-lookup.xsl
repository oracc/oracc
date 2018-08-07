<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:xh="http://www.w3.org/1999/xhtml"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:sl="http://oracc.org/ns/sl/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:key name="signs" match="xh:div[@class='lex-sign']" use="@title"/>

<xsl:template match="sl:sign|sl:form">
  <xsl:variable name="n" select="@n"/>
  <xsl:for-each select="document('/Users/stinney/orc/dcclt/sign-groups.xhtml')">
    <xsl:for-each select="key('signs',$n)">
      <xsl:message>found <xsl:value-of select="$n"/></xsl:message>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>


</xsl:stylesheet>
