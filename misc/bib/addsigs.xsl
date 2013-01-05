<xsl:stylesheet 
    xmlns="http://www.tei-c.org/ns/1.0"
    xmlns:t="http://www.tei-c.org/ns/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0">

<xsl:include href="map-jrnser.xsl"/>
<xsl:include href="map-name.xsl"/>

<xsl:variable name="c10e-from" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>
<xsl:variable name="c10e-to"   select="'abcdefghijklmnopqrstuvwxyz'"/>

<xsl:template match="t:biblStruct">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
    <xsl:call-template name="addsig"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template name="addsig">
  <xsl:variable name="auth1" select=".//t:author"/>
  <xsl:if test="string-length($auth1)>0">
    <xsl:variable name="nmkey">
      <xsl:call-template name="map-name">
	<xsl:with-param name="nm" select=".//t:author[1]"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="t:analytic">
	<xsl:variable name="jskey">
	  <xsl:choose>
	    <xsl:when test="t:series">
	      <xsl:call-template name="map-jrnser">
		<xsl:with-param name="js" select="t:series/t:title"/>
	      </xsl:call-template>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="t:monogr/t:title"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="nv" select="(t:monogr/t:imprint/
	                                 t:biblScope[@type='vol']|
					 t:series/t:biblScope[@type='number'])[1]"/>
	<xsl:variable name="pages" select="t:monogr/t:imprint/t:biblScope[@type='pages']"/>
	<xsl:variable name="pg1">
	  <xsl:choose>
	    <xsl:when test="contains($pages,'-')">
	      <xsl:value-of select="substring-before($pages,'-')"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$pages"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<note type="signature">
	  <xsl:value-of select="concat($nmkey,'#',$jskey,'#',$nv,'#',$pg1)"/>
	</note>
      </xsl:when>
      <xsl:otherwise>
	<xsl:if test="string-length(.//date[1])>0">
	  <xsl:variable name="title">
	    <xsl:call-template name="c10e">
	      <xsl:with-param name="t" select=".//t:title[0]"/>
	    </xsl:call-template>
	  </xsl:variable>
	  <note type="signature">
	    <xsl:value-of select="concat($nmkey,'#',.//t:date[1],'#',$title)"/>
	  </note>
	</xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<xsl:template name="c10e">
  <xsl:param name="t"/>
  <xsl:value-of select="translate(normalize-spaces($t),$c10e-from,$c10e-to)"/>
</xsl:template>

</xsl:stylesheet>