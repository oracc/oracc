<xsl:transform xmlns:g="http://oracc.org/ns/gdl/1.0"
	       xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       version="1.0">

  <!-- Simple script to output top level of word children; primarily
       intended for use with PCTC, and possibly only useful for that,
       in fact -->
  
  <xsl:output method="text" encoding="UTF-8"/>

  <xsl:template match="g:w">
    <xsl:variable name="wid" select="@xml:id"/>
    <xsl:for-each select="*">
      <xsl:variable name="text">
	<xsl:choose>
	  <xsl:when test="@form">
	    <xsl:value-of select="@form"/>
	  </xsl:when>
	  <xsl:when test="self::g:gg">
	    <xsl:choose>
	      <xsl:when test="@g:type='correction'">
		<xsl:for-each select="*[1]">
		  <xsl:choose>
		    <xsl:when test="@form"><xsl:value-of select="@form"/></xsl:when>
		    <xsl:otherwise><xsl:value-of select="text()"/></xsl:otherwise>
		  </xsl:choose>
		</xsl:for-each>
	      </xsl:when>
	      <xsl:when test="@g:type='ligature'">
		<xsl:for-each select="*">
		  <xsl:text>|</xsl:text>
		  <xsl:choose>
		    <xsl:when test="@form"><xsl:value-of select="@form"/></xsl:when>
		    <xsl:otherwise><xsl:value-of select="text()"/></xsl:otherwise>
		  </xsl:choose>
		  <xsl:if test="not(position()=last())"><xsl:text>+</xsl:text></xsl:if>
		</xsl:for-each>
		<xsl:text>|</xsl:text>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:message>word-children.xsl: unhandled g:gg type '<xsl:value-of select="@g:type"/>'</xsl:message>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="text()"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:variable>
      <xsl:if test="not($text='N') and not($text='X') and not($text='...')">
	<xsl:value-of select="concat($wid,'&#x9;',$text,'&#xa;')"/>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>

  <xsl:template match="text()"/>
</xsl:transform>
