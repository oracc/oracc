<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" encoding="utf-8"/>

<xsl:param name="project"/>

<xsl:strip-space elements="*"/>

<xsl:template match="search">
  <xsl:if test="count(ancestor::search)=0">
    <xsl:value-of select="concat('#',$project,' ')"/>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="@type='simple'">
      <xsl:choose>
	<xsl:when test="string-length(../glossary)>0 and not(../glossary='#none')">
	  <xsl:value-of select="concat('!cbd/',../glossary,' ')"/>	  
	</xsl:when>
	<xsl:otherwise>
	  <xsl:if test="not(starts-with(toks,'!'))">
	    <xsl:choose>
	      <xsl:when test="string-length(../default-index)>0">
		<xsl:value-of select="concat('!',../default-index,' ')"/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:text>!txt </xsl:text>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:if>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:value-of select="toks"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="count(preceding-sibling::search) > 0"> 
	<xsl:value-of select="concat(@type, ' ')"/>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<!--
<xsl:template match="project[not(../search[@type='simple'])]">
  <xsl:value-of select="concat('#',.,' ')"/>
</xsl:template>
-->

<xsl:template match="index">
  <xsl:value-of select="concat('!',.,' ')"/>
</xsl:template>

<xsl:template match="binop">
  <xsl:value-of select="concat(' ', @type, ' ')"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="near">
  <xsl:value-of select="concat(' ;', @where, @scope, @level, ' ')"/>
</xsl:template>

<xsl:template match="key">
  <xsl:text>(</xsl:text>
  <xsl:variable name="index" select="ancestor::search/index/text()"/>
  <xsl:choose>
    <xsl:when test="$index='txt'">
      <xsl:for-each select="following-sibling::txt-filters/*">
	<xsl:if test="not(.='any')">
	  <xsl:value-of select="concat(.,':')"/>
	</xsl:if>
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="$index='lem'">
      <xsl:if test="following-sibling::lem-filters/final = 'true'">
	<xsl:value-of select="concat('we',':')"/>
      </xsl:if>
      <xsl:for-each select="following-sibling::lem-filters/*[not(self::final)]">
	<xsl:if test="not(.='any')">
	  <xsl:value-of select="concat(.,':')"/>
	</xsl:if>
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="$index='cat'">
    </xsl:when>
    <xsl:when test="$index='tra'">
    </xsl:when>
    <xsl:otherwise>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:value-of select="concat(.,') ')"/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
