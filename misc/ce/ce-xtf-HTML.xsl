<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:ce="http://oracc.org/ns/ce/1.0"
		exclude-result-prefixes="ce">

<xsl:include href="html-standard.xsl"/>
<xsl:include href="g2-gdl-HTML.xsl"/>
<xsl:param name="project"/>

<xsl:template match="ce:ce">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="webtype" select="'oraccscreen'"/>
    <xsl:with-param name="p2" select="'yes'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <xsl:choose>
    <xsl:when test="@kwic='yes'">
      <table>
	<xsl:apply-templates mode="kwic"/>
      </table>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="ce:l"/>

<xsl:template mode="kwic" match="ce:data">
  <tr>
    <xsl:apply-templates mode="kwic"/>
  </tr>
</xsl:template>

<xsl:template mode="kwic" match="ce:kwic1|ce:kwic2|ce:kwic3">
  <td class="{local-name(.)}">
    <xsl:apply-templates/>
  </td>
</xsl:template>

<xsl:template mode="kwic" match="ce:label">
  <tr>
    <td colspan="3" class="ce-label">
      <xsl:apply-templates/>
    </td>
  </tr>
</xsl:template>

<xsl:template match="ce:data">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="ce:content">
  <p class="ce-result">
    <xsl:apply-templates/>
  </p>
</xsl:template>

<xsl:template match="ce:label">
  <p class="ce-label">
    <xsl:apply-templates/>
  </p>
</xsl:template>

</xsl:stylesheet>
