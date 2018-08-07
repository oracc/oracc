<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
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
  <xsl:if test="xcl:d[@subtype='sg']">
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
	<xsl:value-of select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='sg']]/xff:f/@form"/>
      </xsl:attribute>
      <xsl:attribute name="read">
	<xsl:value-of select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='sv']]/xff:f/@form"/>
      </xsl:attribute>
      <xsl:attribute name="spel">
	<xsl:value-of select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='pr']]/xff:f/@form"/>
      </xsl:attribute>
      <xsl:for-each select="xcl:l[preceding-sibling::xcl:d[@type='field-start'][1][@subtype='sv']]/xff:f">
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
      </xsl:for-each>
    </lex:data>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
