<xsl:stylesheet 
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    version="1.0">

<xsl:output method="text" encoding="utf-8" indent="yes"/>

<xsl:variable name="apos">
  <xsl:text>'</xsl:text>
</xsl:variable>

<xsl:template match="/">
  <xsl:text>\begingroup\cuneifiedstyle
</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>\endgroup
</xsl:text>
</xsl:template>

<xsl:template match="xtf:transliteration|xtf:composite">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:l">
  <xsl:text>\cuneifyLine{</xsl:text>
  <xsl:variable name="num" select="number(translate(@n,$apos,''))"/>
  <xsl:if test="count(preceding-sibling::xtf:l) = 0
		or $num mod  5 = 0
		or $num mod 10 = 0">
    <xsl:value-of select="@label"/>
  </xsl:if>
  <xsl:text>}{</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}
</xsl:text>
</xsl:template>

<xsl:template match="gdl:w">
  <xsl:for-each select=".//*[@gdl:utf8]">
    <xsl:if test="contains(@gdl:o,'[')">
      <xsl:text>\cuneifySquare{</xsl:text>
    </xsl:if>
    <xsl:choose>
      <xsl:when test="@gdl:break='missing'">
	<xsl:choose>
	  <xsl:when test="contains(@gdl:utf8,'&#x2591;')">
	    <xsl:value-of select="@gdl:utf8"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <span class="broken">
	      <xsl:value-of select="@gdl:utf8"/>
	    </span>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="@gdl:utf8"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="contains(@gdl:c,']')">
      <xsl:text>}</xsl:text>
    </xsl:if>
  </xsl:for-each>
  <xsl:if test="not(@form='x') and not(@form='(xx)')
		and not('x'=substring(@form,string-length(@form)))
		and not('x)'=substring(@form,string-length(@form)-1))
		and not(position()=last())">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="gdl:nonw">
  <xsl:for-each select=".//*[@gdl:utf8]">
    <xsl:value-of select="@gdl:utf8"/>
  </xsl:for-each>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
