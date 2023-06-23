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

  <xsl:template name="menu">
    <xsl:param name="menu-page"/>
    <xsl:param name="current-page" tunnel="yes"/>
    <xsl:param name="first-link-page"/>
    <ul>
      <xsl:if test="$first-link-page">
	<li class="only">
	  <xsl:choose>
	    <xsl:when test="$first-link-page/@id = $current-page/@id">
	      <div id="SelfInMenu" class="only"><xsl:value-of select="$first-link-page/esp:name"/></div>
	    </xsl:when>
	    <xsl:otherwise>
	      <esp:link page="{$first-link-page/@id}" class="only">
		<xsl:copy-of select="@type"/>
	      </esp:link>
	    </xsl:otherwise>
	  </xsl:choose>
	</li>
      </xsl:if>
      <xsl:for-each select="$menu-page/struct:page[not ( @hide-menu-link = 'yes' )]">
	<xsl:variable name="is-open" select="descendant-or-self::struct:page[@id = $current-page/@id] and descendant::struct:page"/>
	<xsl:variable name="li-class">
	  <xsl:choose>
	    <xsl:when test="$is-open">open</xsl:when>
	    <xsl:otherwise>
	      <xsl:choose>
		<xsl:when test="descendant::struct:page">closed</xsl:when>
		<xsl:otherwise>only</xsl:otherwise>
	      </xsl:choose>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<li class="{$li-class}">
	  <xsl:choose>
	    <xsl:when test="@id = $current-page/@id">
	      <div id="SelfInMenu" class="{$li-class}"><xsl:value-of select="esp:name"/></div>
	    </xsl:when>
	    <xsl:otherwise>
	      <esp:link page="{@id}" class="{$li-class}">
		<xsl:copy-of select="@type"/>
	      </esp:link>
	    </xsl:otherwise>
	  </xsl:choose>
	  <xsl:if test="$is-open">
	    <xsl:call-template name="menu">
	      <xsl:with-param name="menu-page" select="."/>
	    </xsl:call-template>
	  </xsl:if>
	</li>
      </xsl:for-each>
    </ul>
  </xsl:template>

</xsl:stylesheet>
