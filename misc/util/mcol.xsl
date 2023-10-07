<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0">

<!-- Include mcol.xsl and call it to process the param nodeset in columns.
     In caller .xsl define templates to handle cells by defining them in mode mcol
     -->
<xsl:template name="mcol">
  <xsl:param name="columns"/>
  <xsl:param name="nodes"/>
  <xsl:message>mcol setting <xsl:value-of select="count($nodes)"/> nodes in <xsl:value-of select="$columns"/> colums</xsl:message>
  <table>
    <xsl:apply-templates select="$nodes[(position() - 1) mod $columns = 0]" mode="mcol-first">
      <xsl:with-param name="columns" select="$columns"/>
    </xsl:apply-templates>
  </table>
</xsl:template>

<xsl:template match="*" mode="mcol-first">
  <xsl:param name="columns"/>
  <tr>
    <xsl:apply-templates mode="mcol" select=".|following-sibling::*[position() &lt; $columns]"/>
    <xsl:if test="count(following-sibling::*) &lt; ($columns - 1)">
      <xsl:call-template name="emptycell">
        <xsl:with-param name="cells" select="$columns - 1 - count(following-sibling::cd)"/>
      </xsl:call-template>
    </xsl:if>
  </tr>
</xsl:template>

<xsl:template name="emptycell">
  <xsl:param name="cells"/>
  <td/>
  <xsl:if test="$cells &gt; 1">
    <xsl:call-template name="emptycell">
      <xsl:with-param name="cells" select="$cells - 1"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

</xsl:transform>
