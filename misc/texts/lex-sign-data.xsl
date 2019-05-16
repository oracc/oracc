<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:xcl="http://oracc.org/ns/xcl/1.0"
		xmlns:xff="http://oracc.org/ns/xff/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xcl xff">

<xsl:template match="/">
  <lex:text>
    <xsl:apply-templates select=".//xcl:c[@type='sentence']"/>
  </lex:text>
</xsl:template>

<xsl:template match="xcl:c">
  <xsl:variable name="has-spel"
		select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='pr']]/xff:f/@form"/>
  <!--<xsl:message>spel=<xsl:value-of select="$has-spel"/></xsl:message>-->
  <xsl:variable name="has-pr"
		select="string-length(
			xcl:l
			[count(id(@ref)//g:v[not(@g:status='implied')])>0]
			[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='pr']]/xff:f/@form)"/>
  <xsl:if test="xcl:d[@subtype='sg'] and string-length($has-spel)>0"> <!-- and $has-pr > 0 -->
    <lex:data>
      <xsl:copy-of select="/*/@project"/>
      <xsl:attribute name="id_text">
	<xsl:value-of select="/*/@xml:id"/>
      </xsl:attribute>
      <xsl:copy-of select="/*/@n"/>
      <xsl:copy-of select="@xml:id"/>
      <xsl:copy-of select="@label"/>
      <xsl:attribute name="sref">
	<xsl:value-of select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='sg']]/@ref"/>
      </xsl:attribute>
      <xsl:attribute name="sign">
	<xsl:call-template name="sg"/>
      </xsl:attribute>
      <xsl:attribute name="read">
	<xsl:value-of select="xcl:l[count(id(@ref)//g:v[not(@g:status='implied')])>0][preceding-sibling::xcl:d[@type='field-start'][1][@subtype='sv']]/xff:f/@form"/>
      </xsl:attribute>
      <xsl:attribute name="spel">
	<xsl:value-of select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='pr']]/xff:f/@form"/>
      </xsl:attribute>
      <xsl:for-each select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='sv']]/xff:f">
	<xsl:if test="count(id(../@ref)//g:v[not(@g:status='implied')])>0">
<!--	  <xsl:message><xsl:value-of select="ancestor::xcl:c[@type='sentence']/@label"/>::<xsl:value-of select="../@ref"/></xsl:message> -->
	  <xsl:choose>
	    <xsl:when test="string-length(@cf)>0">
	      <xsl:attribute name="wref">
		<xsl:value-of select="../@ref"/>
	      </xsl:attribute>
	      <xsl:attribute name="word">
		<xsl:value-of select="concat(@cf,'[',@gw,']',@pos)"/>
	      </xsl:attribute>
	      <xsl:attribute name="base">
		<xsl:value-of select="@base"/>
	      </xsl:attribute>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:attribute name="pos">
		<xsl:value-of
		    select="@pos"/>
	      </xsl:attribute>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:if>
      </xsl:for-each>
    </lex:data>
  </xsl:if>
</xsl:template>

<xsl:template name="sg">
  <xsl:variable name="f-node"
		select="id(xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='sg']]/@ref)"/>
  <xsl:choose>
    <xsl:when test="$f-node/../g:nonw[@type='comment']">
      <xsl:value-of select="$f-node/../g:nonw[@type='comment']"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='sg']]/xff:f/@form"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
