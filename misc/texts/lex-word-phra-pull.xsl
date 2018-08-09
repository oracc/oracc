<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xi">

<xsl:template match="lex:dataset">
  <lex:phrases>
    <xsl:apply-templates/>
  </lex:phrases>
</xsl:template>

<xsl:template match="lex:text">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="lex:data">
  <xsl:for-each select="*[1]">
    <xsl:if test="self::lex:sv or self::lex:wp">
      <xsl:if test="count(*)>1">
	<xsl:variable name="head">
	  <xsl:for-each select="*[1]">
	    <xsl:choose>
	      <xsl:when test="@cfgw">
		<xsl:value-of select="@cfgw"/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="@form"/>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:for-each>
	</xsl:variable>
	<xsl:variable name="phrase">
	  <xsl:for-each select="*">
	    <xsl:choose>
	      <xsl:when test="@cfgw">
		<xsl:value-of select="@cfgw"/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="@form"/>
	      </xsl:otherwise>
	    </xsl:choose>
	    <xsl:if test="not(position()=last())">
	      <xsl:text> </xsl:text>
	    </xsl:if>
	  </xsl:for-each>
	</xsl:variable>	
	<xsl:variable name="eq"
		      select="following-sibling::lex:eq[starts-with(lex:word[1]/@lang,'akk')][1]/*"/>
	<xsl:variable name="equiv">
	  <xsl:choose>
	    <xsl:when test="$eq[@cfgw]">
	      <xsl:for-each select="$eq">
		<xsl:choose>
		  <xsl:when test="@cfgw">
		    <xsl:value-of select="@cfgw"/>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:value-of select="@form"/>
		  </xsl:otherwise>
		</xsl:choose>
		<xsl:if test="not(position()=last())">
		  <xsl:text> </xsl:text>
		</xsl:if>
	      </xsl:for-each>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text>#none</xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>	
	<lex:phrase lang="{*[1]/@lang}" head="{$head}" phrase="{$phrase}" equiv="{$equiv}">
	  <xsl:copy-of select="ancestor::lex:data"/>
	</lex:phrase>
      </xsl:if>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
