<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
		version="1.0" 
		xmlns:g="http://oracc.org/ns/gdl/1.0">

<xsl:template name="hethify">
  <xsl:param name="text"/>
  <xsl:call-template name="render-g-text">
    <xsl:with-param name="t" select="translate($text,'hH','ḫḪ')"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="render-g">
  <xsl:variable name="body">
    <xsl:choose>
      <xsl:when test="$render-accents = 'yes' and @g:accented">
	<xsl:value-of select="@g:accented"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:choose>
	  <xsl:when test="g:b">
	    <xsl:value-of select="g:b"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="text()"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:call-template name="hethify">
    <xsl:with-param name="text" select="$body"/>
  </xsl:call-template>
  <xsl:if test="g:b">
    <xsl:apply-templates select="g:m|g:a"/>
  </xsl:if>
<!--
  <xsl:if test="@notemark">
    <xsl:call-template name="process-notes"/>
  </xsl:if>
 -->
</xsl:template>

</xsl:stylesheet>