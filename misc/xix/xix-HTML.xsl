<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xix="http://oracc.org/ns/xix/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xix cbd">

<xsl:import href="html-standard.xsl"/>
<xsl:include href="g2-gdl-HTML.xsl"/>

<xsl:param name="project"/>
<xsl:param name="with-count" select="'no'"/>
<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:variable name="lower" select="'abcdefghijklmnopqrstuvwxyz'"/>
<xsl:variable name="upper" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>

<xsl:template match="/">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="title" select="'XIX'"/>
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="webtype" select="'cbd'"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
  <xsl:if test="count(*/xix:letter) = 0">
    <div class="tocbanner">
      <xsl:text>ePSD </xsl:text>
      <xsl:value-of select="translate(/*/@title,' ','&#xa0;')"/>
      <xsl:if test="$with-count = 'yes'">
	<xsl:value-of select="concat(' (', count(/*/xix:ix), ' entries)')"/>
      </xsl:if>
    </div>
  </xsl:if>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xix:letter">
  <div class="tocbanner">
    <p class="toctitle">
      <xsl:text>ePSD </xsl:text>
      <xsl:value-of select="translate(ancestor::xix:index/@title,' ','&#xa0;')"/>
    </p>
    <p class="tocletter"><xsl:apply-templates/></p>
  </div>
</xsl:template>

<xsl:template match="xix:ix">
  <p class="ix">
    <span class="what"><xsl:apply-templates select="xix:what"/></span>
    <xsl:choose>
      <xsl:when test="xix:where">
        <span class="where">
          <xsl:for-each select="xix:where">
            <xsl:variable name="js">
              <xsl:text>'</xsl:text>
              <xsl:value-of select="@ref"/>
              <xsl:text>.html'</xsl:text>
            </xsl:variable>
	    <a href="javascript:showarticle({$js})"><xsl:apply-templates/></a>
  	    <xsl:if test="not(position() = last())">
  	      <xsl:text>; </xsl:text>
            </xsl:if>
          </xsl:for-each>
        </span>
      </xsl:when>
      <xsl:otherwise>
        <xsl:for-each select="cbd:summary">
          <xsl:variable name="js">
            <xsl:text>'</xsl:text>
            <xsl:value-of select="@ref"/>
            <xsl:text>.html'</xsl:text>
          </xsl:variable>
  	  <span class="summary">
	    <br/>
	    <a href="javascript:showarticle({$js})">
	      <xsl:apply-templates select="cbd:cf|cbd:gw"/>
	    </a>
	    <xsl:call-template name="bases"/>
	    <xsl:apply-templates select="cbd:mng|cbd:term"/>
  	  </span>
 	</xsl:for-each>
      </xsl:otherwise>
    </xsl:choose>
  </p>
</xsl:template>

<xsl:template match="cbd:cf">
  <span class="ixcf"><xsl:apply-templates/></span>
</xsl:template>

<xsl:template match="cbd:gw">
  <xsl:text> [</xsl:text>
  <span class="gw"><xsl:value-of select="translate(text(),$lower,$upper)"/></span>
  <xsl:text>]</xsl:text>
</xsl:template>

<xsl:template name="bases">
  <xsl:if test="cbd:base">
    <xsl:text> wr. </xsl:text>
    <xsl:for-each select="cbd:base">
      <span class="wr"><xsl:apply-templates select="cbd:text"/></span>
      <xsl:if test="not(position() = last())">
	<xsl:text>; </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:if>
</xsl:template>

<xsl:template match="cbd:mng">
  <xsl:choose>
    <xsl:when test="not(preceding-sibling::cbd:mng)">
      <xsl:text> "</xsl:text>
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>; </xsl:text>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="not(following-sibling::cbd:mng)">
    <xsl:text>"</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="cbd:term">
  <xsl:choose>
    <xsl:when test="not(preceding-sibling::cbd:term)">
      <xsl:text> Akk.&#xa0;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>; </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <span class="akk"><xsl:apply-templates/></span>
  <xsl:if test="not(following-sibling::cbd:term)">
<!--    <xsl:text>.</xsl:text> -->
  </xsl:if>
</xsl:template>

<xsl:template match="cbd:sup|xix:sup">
  <sup><xsl:apply-templates/></sup>
</xsl:template>

</xsl:stylesheet>
