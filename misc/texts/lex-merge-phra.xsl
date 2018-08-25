<?xml version='1.0'?>
<xsl:stylesheet version="1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:o="http://oracc.org/ns/oracc/1.0"
		xmlns:xh="http://www.w3.org/1999/xhtml"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="lex">

<xsl:key name="oid" match="xh:div" use="@o:id"/>
<xsl:variable name="tail" select="document('lex-phra-tail.xhtml',/)"/>
  
<xsl:template match="/*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="*">
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:copy-of select="*"/>
	<xsl:variable name="oid" select="@o:id"/>
	<xsl:if test="string-length($oid)>0">
	  <xsl:for-each select="$tail">
<!--	    <xsl:message>looking for <xsl:value-of select="$oid"/></xsl:message> -->
	    <xsl:if test="key('oid',$oid)/@o:id">
<!--	      <xsl:message>found <xsl:value-of select="$oid"/> in tail</xsl:message> -->
	      <hr class="lex"/>
	      <xsl:copy-of select="key('oid',$oid)/*"/>
	    </xsl:if>
	  </xsl:for-each>
	</xsl:if>
      </xsl:copy>
    </xsl:for-each>
    <xsl:variable name="head-node" select="/"/>
    <xsl:for-each select="$tail/*/*">
      <xsl:if test="string-length(@o:id)>0">
	<xsl:variable name="tail-node" select="."/>
	<xsl:for-each select="$head-node">
	  <xsl:choose>
	    <xsl:when test="string-length(key('oid',$tail-node/@o:id))=0">
<!--	      <xsl:message>dumped tail node <xsl:value-of select="$tail-node/@o:id"/></xsl:message> -->
	      <xsl:copy-of select="$tail-node"/>
	    </xsl:when>
	    <xsl:otherwise>
<!--	      <xsl:message>found tail node <xsl:value-of select="$tail-node/@o:id"/> among heads</xsl:message>	       -->
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:for-each>
      </xsl:if>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
