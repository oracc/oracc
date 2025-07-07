<xsl:stylesheet
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"	
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    xmlns:struct="http://oracc.org/ns/esp-struct/1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    version="2.0" 	
    xpath-default-namespace="http://www.w3.org/1999/xhtml"
    >

  <xsl:template name="site-map">
    <xsl:param name="map-page"/>
    <xsl:param name="current-page" tunnel="yes"/>
    <ul>
      <xsl:for-each select="$map-page/struct:page[not ( @hide-site-map-link = 'yes' )]">
	<li>
	  <xsl:variable name="page-level" select="count ( ancestor::struct:page )"/>
	  <xsl:variable name="tag">
	    <xsl:choose>
	      <xsl:when test="$page-level = 1">h2</xsl:when>
	      <xsl:when test="$page-level = 2">h3</xsl:when>
	      <xsl:otherwise>div</xsl:otherwise>
	    </xsl:choose>
	  </xsl:variable>
	  <xsl:element name="{$tag}">
	    <xsl:choose>
	      <xsl:when test="@id = $current-page/@id">
		<xsl:call-template name="outline-numbering"/>
		<xsl:value-of select="esp:title"/>
	      </xsl:when>
	      <xsl:otherwise>
		<esp:link page="{@id}" title="">
		  <xsl:call-template name="outline-numbering"/>
		  <xsl:value-of select="esp:title"/>
		</esp:link>						
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:element>				
	  <xsl:if test="descendant::struct:page[not ( @hide-site-map-link = 'yes' )]">
	    <xsl:call-template name="site-map">
	      <xsl:with-param name="map-page" select="."/>
	    </xsl:call-template>
	  </xsl:if>
	</li>
      </xsl:for-each>
    </ul>
  </xsl:template>
  
  <xsl:template name="outline-numbering">
    <xsl:for-each select="ancestor-or-self::struct:page[ancestor::struct:page]">
      <xsl:value-of select="count ( preceding-sibling::struct:page ) + 1"/>
      <xsl:text>.</xsl:text>
    </xsl:for-each>
    <xsl:text>&#160;&#160;</xsl:text>
  </xsl:template>

</xsl:stylesheet>
