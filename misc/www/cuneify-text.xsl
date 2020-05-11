<xsl:stylesheet 
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    version="1.0">
<xsl:output method="xml" encoding="utf-8" indent="yes"/>

<xsl:param name="period" select="'na'"/>

<xsl:variable name="apos">
  <xsl:text>'</xsl:text>
</xsl:variable>

<xsl:template match="/">
  <html>
    <head>
      <title>Cuneified <xsl:value-of select="/*/@n"/></title>
      <link rel="stylesheet" type="text/css" href="/css/cuneify.css" />
      <link rel="stylesheet" type="text/css" href="/css/cuneify-{$period}.css" />
    </head>
    <body>
      <xsl:apply-templates select="xtf:transliteration|xtf:composite
				   |*/xtf:transliteration|*/xtf:composite"/>
    </body>
  </html>
</xsl:template>

<xsl:template match="xtf:transliteration|xtf:composite">
  <h1>Cuneified <xsl:value-of select="@n"/></h1>
  <p class="disclaimer">[This cuneiform text was computer-generated
  from a transliteration. <a href="/cuneify-trouble.html">See this page for trouble-shooting tips.</a>]</p>
  <table class="cuneify-text">
    <xsl:apply-templates/>
  </table>
</xsl:template>

<xsl:template match="xtf:l">
  <tr class="cuneify-line cuneiform">
    <td class="cuneify-label">
      <xsl:variable name="num" select="number(translate(@n,$apos,''))"/>
      <xsl:choose>
	<xsl:when test="count(preceding-sibling::xtf:l) = 0
			or $num mod  5 = 0
			or $num mod 10 = 0">
			<xsl:value-of select="@label"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>&#xa0;</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </td>
    <td>
      <p class="cuneify-content">
	<xsl:apply-templates/>
      </p>
    </td>
  </tr>
</xsl:template>

<xsl:template match="gdl:w">
  <xsl:for-each select=".//*[@gdl:utf8]">
    <xsl:if test="contains(@gdl:o,'[')"><span class="osquare"><![CDATA[]]></span></xsl:if>
    <xsl:choose>
      <xsl:when test="@gdl:break='missing'">
	<xsl:choose>
	  <xsl:when test="contains(@gdl:utf8,'&#x2591;')">
	    <xsl:value-of select="@gdl:utf8"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <span class="broken">
	      <xsl:choose>
		<xsl:when test="@gdl:utf8='x'">
		  <span class="roman">x</span>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:value-of select="@gdl:utf8"/>
		</xsl:otherwise>
	      </xsl:choose>
	    </span>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="@gdl:utf8"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="contains(@gdl:c,']')"><span class="csquare"><![CDATA[]]></span></xsl:if>
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

<xsl:template match="xtf:note"/>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
