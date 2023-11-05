<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns="http://www.w3.org/1999/xhtml"
    version="1.0">

<!-- Include mcol.xsl and call it to process the param nodeset in columns.
     In caller .xsl define templates to handle cells by defining them in mode mcol
     -->
<xsl:template name="mcol">
  <xsl:param name="columns"/>
  <xsl:param name="nodes"/>
  <xsl:param name="width"/>
  <xsl:param name="class"/>
  <xsl:param name="tag" select="'sign'"/>
  <!--<xsl:message>mcol setting <xsl:value-of select="count($nodes)"/> nodes in <xsl:value-of select="$columns"/> columns</xsl:message>-->
  <table>
    <xsl:if test="string-length($width)>0"><xsl:attribute name="width"><xsl:value-of select="$width"/></xsl:attribute></xsl:if>
    <xsl:if test="string-length($class)>0"><xsl:attribute name="class"><xsl:value-of select="$class"/></xsl:attribute></xsl:if>
    <xsl:for-each select="$nodes[local-name()=$tag][(position() - 1) mod $columns = 0]">
      <!--<xsl:message>mcol first @n = <xsl:value-of select="@n"/></xsl:message>-->
      <xsl:apply-templates select="." mode="mcol-first">
	<xsl:with-param name="columns" select="$columns"/>
	<xsl:with-param name="tag" select="$tag"/>
      </xsl:apply-templates>
    </xsl:for-each>
  </table>
</xsl:template>

<xsl:template match="*" mode="mcol-first">
  <xsl:param name="columns"/>
  <xsl:param name="tag"/>
  <tr>
    <xsl:apply-templates mode="mcol"
			 select=".|following-sibling::*[local-name()=$tag][position() &lt; $columns]"/>
    <xsl:if test="count(following-sibling::*) &lt; ($columns - 1)">
      <xsl:call-template name="emptycell">
        <xsl:with-param name="cells" select="$columns - 1 - count(following-sibling::*[local-name()=$tag])"/>
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
