<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:sl="http://oracc.org/ns/sl/1.0">
<xsl:output method="text" encoding="utf-8"/>
<xsl:variable name="prefix" select="'urn:oracc:scr:Xsux:'"/>
<xsl:template match="sl:sign">
  <xsl:value-of select="concat($prefix,'sn:', translate(@n,'|',''), '&#x9;', @uname, '&#xa;')"/>
  <xsl:for-each select="sl:form">
    <xsl:value-of select="concat($prefix,'sign:', translate(ancestor::sl:sign/@n,'|',''), 
			  ':vf:', translate(@n,'|',''), '&#xa;')"/>
    <xsl:variable name="form" select="@n"/>
    <xsl:for-each select="ancestor::sl:sign/sl:v">
      <xsl:value-of select="concat($prefix,'sn:', translate(ancestor::sl:sign/@n,'|',''), ':vf:', 
			    translate($form,'|',''), ':rd:', translate(@n,':/','&#xffa1;&#x2044;'), '(', 
			    translate($form,'|',''),')', '&#xa;')"/>
    </xsl:for-each>
  </xsl:for-each>
  <xsl:for-each select="sl:v">
    <xsl:value-of select="concat($prefix,'sn:', 
			  translate(ancestor::sl:sn/@n,'|',''), 
			  ':rd:', translate(@n,':/','&#xff1a;&#x2044;'), 
			  '&#xa;')"/>
  </xsl:for-each>
</xsl:template>

</xsl:transform>
