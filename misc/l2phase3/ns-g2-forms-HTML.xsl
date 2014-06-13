<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xpd="http://oracc.org/ns/xpd/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:i="http://oracc.org/ns/instances/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:note="http://oracc.org/ns/note/1.0"
  xmlns:n="http://oracc.org/ns/norm/1.0"
  xmlns:norm="http://oracc.org/ns/norm/1.0"
  xmlns:usg="http://oracc.org/ns/usg/1.0"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:oracc="http://oracc.org/ns/oracc/1.0"
  exclude-result-prefixes="dc i xl xcl norm usg note">

<xsl:include href="g2-gdl-HTML.xsl"/>

<xsl:key name="norms" match="cbd:norm/cbd:forms/cbd:f" use="@ref"/>

<xsl:param name="basename" select="/*/*/@project"/>
<xsl:param name="project" select="/*/*/@project"/>

<xsl:template match="/">
  <body>
    <div class="wrapper">
      <xsl:for-each select="*/@n|*/@project|*/@xml:lang">
	<xsl:attribute name="oracc:{local-name()}"><xsl:value-of select="."/></xsl:attribute>
      </xsl:for-each>
      <xsl:apply-templates/>
    </div>
  </body>
</xsl:template>

<xsl:template match="cbd:entry">
  <div class="body">
    <xsl:attribute name="xml:id"><xsl:value-of select="concat(@xml:id,'.f')"/></xsl:attribute>
    <div class="banner bborder-top bborder-bot">
      <p class="center">
	<xsl:value-of select="document($config-file)/*/xpd:abbrev"/>
	<xsl:text>: </xsl:text>
	<xsl:value-of select="document($config-file)/*/xpd:name"/>
      </p>
    </div>
    <div class="ns-glo-sec" id="@xml:id.f">
      <div class="header">
        <h1 class="entry heading border-top">Written forms attested for
	<xsl:text> </xsl:text>
	<span class="cf akk"><xsl:value-of select="cbd:cf"/></span>
	<xsl:text> </xsl:text>
	<span class="gw"><xsl:value-of select="cbd:gw"/></span>
	<xsl:text> </xsl:text>
	<span class="pos"><xsl:value-of select="cbd:pos"/></span>
	</h1>
        <p class="icount">
          <a class="icount" href="javascript:distprof2('{$basename}','{/*/@xml:lang}','{@xis}')">
	    <xsl:text> </xsl:text>
	    <xsl:choose>
	      <xsl:when test="@icount=1">
		1 instance
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="@icount"/> instances
	      </xsl:otherwise>
	    </xsl:choose>
	  </a>
        </p>
      </div>

      <xsl:for-each select="cbd:forms/cbd:form">
	<xsl:variable name="norm-nodes" select="key('norms',@xml:id)"/>
	<p class="ns-glo-sub">
	  <a class="icountu" href="javascript:distprof2('{$basename}','{/*/@xml:lang}','{@xis}')">
	    <xsl:apply-templates select="cbd:t[1]/*"/>
	    <!--<xsl:value-of select="concat(' (',@icount,'x/',@ipct,'%)')"/>-->
	    <xsl:if test="count($norm-nodes)>1">
	      <xsl:value-of select="concat(' [',@icount,'x]')"/>
	    </xsl:if>
	  </a>
	  =
	  <xsl:for-each select="$norm-nodes">
	    <xsl:for-each select="ancestor::cbd:norm">
	      <a class="icountu" href="javascript:distprof2('{$basename}','{/*/@xml:lang}','{@xis}')">
		<xsl:value-of select="cbd:n"/>
		<!--<xsl:value-of select="concat(' (',@icount,'x/',@ipct,'%)')"/>-->
		<xsl:value-of select="concat(' [',@icount,'x]')"/>
	      </a>
	      <xsl:choose>
		<xsl:when test="position()=last()">
		  <xsl:text>.</xsl:text>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:text>; </xsl:text>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:for-each>
	  </xsl:for-each>
	</p>
      </xsl:for-each>
    </div>
  </div>
</xsl:template>

<xsl:template match="cbd:articles|cbd:letter">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
