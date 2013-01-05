<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:c="http://oracc.org/ns/cbd/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:s="http://oracc.org/ns/sortkey/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:ex="http://exslt.org/common"
  extension-element-prefixes="ex"
  exclude-result-prefixes="c g s">

<xsl:key name="enam" match="c:entry" use="@n"/>
<xsl:key name="xcpd" match="c:cpd" use="@eref"/>
<xsl:key name="xsig" match="c:cpd" use="@esig"/>

<xsl:template match="c:entry/c:pos">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
  <xsl:variable name="xcpd" select="key('xcpd',ancestor::c:entry/@xml:id)"/>
  <xsl:choose>
    <xsl:when test="count($xcpd)>0">
      <c:see-compounds>
	<xsl:for-each select="$xcpd">
	  <xsl:for-each select="ancestor::c:entry">
	    <xcpd eref="{@xml:id}">
	      <xsl:value-of select="concat(c:cf,'[',c:gw,']',c:pos)"/>
	    </xcpd>
	  </xsl:for-each>
	</xsl:for-each>
      </c:see-compounds>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="xsig" select="key('xsig',ancestor::c:entry/@n)"/>
      <xsl:if test="count($xsig)>0">
	<c:see-compounds>
	  <xsl:for-each select="$xsig">
	    <xsl:variable name="primary" select="@primary"/>
	    <xsl:for-each select="ancestor::c:entry">
	      <xcpd eref="{@xml:id}" primary="{$primary}">
		<xsl:value-of select="@n"/>
	      </xcpd>
	    </xsl:for-each>
	  </xsl:for-each>
	</c:see-compounds>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="c:cpd">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:variable name="eref" select="key('enam',@esig)/@xml:id"/>
    <xsl:if test="string-length($eref) > 0">
      <xsl:attribute name="eref"><xsl:value-of select="$eref"/></xsl:attribute>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>