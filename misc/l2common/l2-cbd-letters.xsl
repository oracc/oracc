<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://oracc.org/ns/cbd/1.0"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" encoding="utf-8" indent="no"/>

<xsl:key name="groups" match="/*/xh:body/xh:p/*[1]/*[1]/xh:a|/*/*"
	use="translate(substring(*[1]/text(),1,1),
		       'ABCDEFGHḪIJKLMNOPQRSTUVWXYZŠŊṢṬŚāēīūĀĒĪŪâêîû',
		       'abcdefghḫijklmnopqrstuvwxyzšŋṣṭśaeiuaeiuaeiu')"/>

<xsl:template name="initials">
  <i l="a" u="A"/>
  <i l="b" u="B"/>
  <i l="c" u="C"/>
  <i l="d" u="D"/>
  <i l="e" u="E"/>
  <i l="f" u="F"/>
  <i l="g" u="G"/>
  <i l="ŋ" u="Ŋ"/>
  <i l="h" u="H"/>
  <i l="ḫ" u="Ḫ"/>
  <i l="i" u="I"/>
  <i l="j" u="J"/>
  <i l="k" u="K"/>
  <i l="l" u="L"/>
  <i l="m" u="M"/>
  <i l="n" u="N"/>
  <i l="o" u="O"/>
  <i l="p" u="P"/>
  <i l="q" u="Q"/>
  <i l="r" u="R"/>
  <i l="s" u="S"/>
  <i l="ṣ" u="Ṣ"/>
  <i l="š" u="Š"/>
  <i l="ś" u="Ś"/>
  <i l="t" u="T"/>
  <i l="ṭ" u="Ṭ"/>
  <i l="u" u="U"/>
  <i l="v" u="V"/>
  <i l="w" u="W"/>
  <i l="x" u="X"/>
  <i l="y" u="Y"/>
  <i l="z" u="Z"/>
</xsl:template>

<xsl:template match="/*">
  <xsl:variable name="doc" select="."/>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="processing-instruction()"/>
    <xsl:choose>
      <xsl:when test="xh:head">
	<xsl:copy-of select="xh:head"/>
	<xsl:apply-templates select="xh:body">
	  <xsl:with-param name="bdoc" select="/*"/>
	</xsl:apply-templates>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="do-children">
	  <xsl:with-param name="doc" select="$doc"/>
	</xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:body">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="*[1]"/>
    <xsl:call-template name="do-children" select="*[position()>1]">
      <xsl:with-param name="doc" select="/*"/>
    </xsl:call-template>  
  </xsl:copy>
</xsl:template>

<xsl:template name="do-children">
  <xsl:param name="doc"/>
  <xsl:for-each select="document('')/*/xsl:template[@name='initials']/*">
    <xsl:variable name="l" select="@l"/>
    <xsl:variable name="have-children">
      <xsl:for-each select="$doc">
	<xsl:value-of select="count(key('groups',$l)/*)"/>
      </xsl:for-each>
    </xsl:variable>
    <xsl:if test="$have-children > 0">
      <xsl:choose>
	<xsl:when test="$doc/xh:body">
	  <div class="letter">
	    <h1 class="obf-letter"><xsl:value-of select="@u"/></h1>
	    <xsl:for-each select="$doc">
	      <xsl:apply-templates mode="nobody" select="key('groups',$l)/ancestor::xh:p"/>
	    </xsl:for-each>
	  </div>
	</xsl:when>
	<xsl:otherwise>
	  <letter dc:title="{@u}">
	    <xsl:for-each select="$doc">
	      <xsl:for-each select="key('groups',$l)">
		<xsl:copy-of select="."/>
	      </xsl:for-each>
	    </xsl:for-each>
	  </letter>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:if>
  </xsl:for-each>    
</xsl:template>

<xsl:template mode="nobody" match="xh:*">
  <xsl:copy-of select="."/>
</xsl:template>

</xsl:stylesheet>
