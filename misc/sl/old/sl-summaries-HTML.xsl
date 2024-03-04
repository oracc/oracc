<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:sl="http://oracc.org/ns/sl/1.0"
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:ex="http://exslt.org/common"
		exclude-result-prefixes="sl"
		extension-element-prefixes="ex"
		>

<xsl:import href="html-standard.xsl"/>
<xsl:import href="gdl-HTML.xsl"/>

<xsl:param name="project" select="'ogsl'"/>
<xsl:param name="title" select="'OGSL'"/>

<xsl:template match="sl:sign">
  <xsl:variable name="file">
    <xsl:value-of select="concat('signlist/02www/',ancestor::sl:letter/@xml:id,'/',@xml:id,'/brief.html')"/>
  </xsl:variable>
  <xsl:message>Generating <xsl:value-of select="$file"/></xsl:message>
  <ex:document href="{$file}"
	       method="xml" encoding="utf-8"
	       indent="yes">
    <xsl:call-template name="make-html">
      <xsl:with-param name="title" select="$title"/>
      <xsl:with-param name="project" select="$project"/>
      <xsl:with-param name="webtype" select="'cbd'"/>
      <xsl:with-param name="with-hr" select="false()"/>
      <xsl:with-param name="with-trailer" select="false()"/>
    </xsl:call-template>
  </ex:document>
</xsl:template>

<xsl:template name="call-back">
  <div class="sl-sign sl-summary">
    <h1 class="sl"><xsl:apply-templates select="sl:name/*"/></h1>
    <p>
      <span class="sl-value"><xsl:text>Values:</xsl:text></span>
      <xsl:for-each select="sl:v">
	<xsl:value-of select="@n"/>
	<xsl:if test="not(position()=last())"><xsl:text>, </xsl:text></xsl:if>
      </xsl:for-each>
      <xsl:text>.</xsl:text>
    </p>
    <xsl:for-each select="sl:v">
      <xsl:if test="sl:glo">
	<h2 class="sl-glo"><xsl:value-of select="@n"/></h2>
	<xsl:for-each select="sl:glo">
	  <xsl:choose>
	    <xsl:when test="sl:glo-group[@type='s']">
	      <xsl:for-each select="sl:glo-group[@type='s']">
		<xsl:for-each select="sl:glo-inst">
		  <p><a href="showsign('{@ref}')"><xsl:value-of select="@cfgw"/></a></p>
		</xsl:for-each>
	      </xsl:for-each>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:if test="sl:glo-group[not(@type='s')]">
		<p>(see full listing)</p>
	      </xsl:if>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:for-each>
      </xsl:if>
    </xsl:for-each>
    <p>Full listing</p>
  </div>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text()"/>
</xsl:stylesheet>
