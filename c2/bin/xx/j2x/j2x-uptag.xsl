<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <!-- For <m><n>name</n><v>value</v></m> shorten to <name>value</name> -->
  <xsl:template match="m">
    <xsl:element name="{n}">
      <xsl:choose>
	<xsl:when test="local-name(*[2]) = 'v'">
	  <xsl:value-of select="v/text()"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:apply-templates select="*[2]"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:element>
  </xsl:template>
  <xsl:template match="*">
    <xsl:copy>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
</xsl:transform>
