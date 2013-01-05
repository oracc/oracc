<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/xff/1.0"
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns:c="http://oracc.org/ns/cbd/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:s="http://oracc.org/ns/sortkey/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:ex="http://exslt.org/common"
  extension-element-prefixes="ex"
  exclude-result-prefixes="c g s">

<xsl:key name="entries" match="c:entry" use="concat(c:cf,'[',c:gw,']',c:pos)"/>
<xsl:key name="entries-cacf" match="c:entry[c:cf/@cacf]" 
	 use="concat(c:cf/@cacf,'[',c:gw,']',c:pos)"/>

<xsl:template match="c:compound">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:for-each select="c:cpd">
      <xsl:choose>
	<xsl:when test="@xml:lang">
	  <!--FIXME: need to cross-validate against external glossary-->
	  <xsl:copy-of select="."/>
	</xsl:when>
	<xsl:when test="string-length(c:gw)=0">
	  <xsl:copy-of select="."/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:variable name="cpdkey" select="concat(c:cf,'[',c:gw,']',c:pos)"/>
	  <xsl:variable name="enode" select="key('entries',$cpdkey)"/>
	  <xsl:choose>
	    <xsl:when test="count($enode) > 0">
	      <xsl:copy>
		<xsl:copy-of select="@*"/>
		<xsl:attribute name="eref">
		  <xsl:value-of select="$enode/@xml:id"/>
		</xsl:attribute>
		<xsl:apply-templates/>
	      </xsl:copy>
	    </xsl:when>
	    <xsl:when test="count(key('entries-cacf',$cpdkey))>0">
	      <xsl:copy>
		<xsl:copy-of select="@*"/>
		<xsl:attribute name="eref">
		  <xsl:value-of select="key('entries-cacf',$cpdkey)/@xml:id"/>
		</xsl:attribute>
		<xsl:apply-templates/>
	      </xsl:copy>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:message><xsl:value-of select="/*/@xml:lang"
	      />.glo: unknown compound part <xsl:value-of select="$cpdkey"
	      /></xsl:message>
	      <xsl:copy-of select="."/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>