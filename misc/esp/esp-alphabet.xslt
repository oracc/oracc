<xsl:stylesheet
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"	
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:esp="http://oracc.org/ns/esp/1.0"
	xmlns="http://www.w3.org/1999/xhtml"
	version="2.0" 	
	xpath-default-namespace="http://www.w3.org/1999/xhtml"
>
<xsl:template name="alphabet">
  <xsl:param name="usedletters" as="xs:string *"/>
  <xsl:variable name="alphabet" as="xs:string *" 
              select="('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 
	               'N', 'O', 'P', 'Q', 'R', 'S', 'Ṣ', 'Š', 'T', 'Ṭ', 'U', 'V', 'W', 
		       'X', 'Y', 'Z', '_')"/>
  <div id="Alphabet">
    <xsl:for-each select="$usedletters">
      <esp:link bookmark="letter_{.}" title="">&#160;<xsl:value-of select="."/>&#160;</esp:link>
      <xsl:text> </xsl:text>
    </xsl:for-each>  
<!--
    <xsl:for-each select="$alphabet">
      <xsl:choose>
        <xsl:when test="empty ( index-of ( $usedletters, . ) )">
          <xsl:text>&#160;</xsl:text><xsl:value-of select="."/><xsl:text>&#160;</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <esp:link bookmark="letter_{.}" title="">&#160;<xsl:value-of select="."/>&#160;</esp:link>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:text> </xsl:text>
    </xsl:for-each>  
 -->
  </div>      
</xsl:template>
</xsl:stylesheet>
