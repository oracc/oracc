<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:xl="http://www.w3.org/1999/xlink"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xix="http://oracc.org/ns/xix/1.0"
  exclude-result-prefixes="xl">

<xsl:import href="html-standard.xsl"/>

<xsl:output method="xml" indent="no" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:variable name="doc" select="/"/>

<xsl:param name="lang"/>
<xsl:param name="project"/>
<xsl:param name="webdir"/>

<xsl:template match="/">
  <xsl:call-template name="make-html">
    <xsl:with-param name="project" select="$project"/>
    <xsl:with-param name="webtype" select="'cbd'"/>
    <xsl:with-param name="title" select="'TOC Matrix'"/>
    <xsl:with-param name="with-hr" select="false()"/>
    <xsl:with-param name="with-trailer" select="false()"/>
    <xsl:with-param name="with-epsd1" select="true()"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="call-back">
<!--  <h1 class="tocmatrix">TOC MATRIX</h1> -->

<!--
    <p class="toces"><a
	    href="javascript:showpage('/epsd/epsd/emesal.html', 'tocbrowser')">
	  <span title="Emesal words"><xsl:text>ES</xsl:text></span>
        </a>
	<xsl:text> | </xsl:text>
	<a href="javascript:showpage('/epsd/epsd/V-simple.html', 'tocbrowser')">
	  <span title="Simple verbs"><xsl:text>V</xsl:text></span>
        </a><xsl:text> | </xsl:text>
	<a href="javascript:showpage('/epsd/epsd/V-compound.html', 'tocbrowser')">
	  <span title="Compound verbs"><xsl:text>CV</xsl:text></span>
        </a><xsl:text> | </xsl:text>
 	<a href="javascript:showpage('/epsd/epsd/supp.html', 'tocbrowser')">
	  <span title="Suppletive verbs"><xsl:text>SV</xsl:text></span>
        </a></p>

     <hr class="toces"/>

 -->

<p class="toces">
  <a href="javascript:showpage('/epsd2/cbd/sux/VS-summaries.html', 'tocbrowser')">
    <xsl:text>Simple Verbs</xsl:text>
  </a>
  <br/>
  <a href="javascript:showpage('/epsd2/cbd/sux/VC-summaries.html', 'tocbrowser')">
    <xsl:text>Compound Verbs Sorted by Noun</xsl:text>
  </a>
  <br/>
  <a href="javascript:showpage('/epsd2/cbd/sux/VCV-summaries.html', 'tocbrowser')">
    <xsl:text>Compound Verbs Sorted by Verb</xsl:text>
  </a>
</p>

     <hr class="toces"/>

  <table class="tocmatrix">

    <tr class="toc-matrix-headers">
  	<td class="toc-matrix-title-cell">
	   <a class="PSD" 
		href="javascript:showpage('../index.html',
					  'tocbrowser')"
		><span class="homebutton">e</span></a></td>
	<td>C</td>
	<td>T</td>
	<td>S</td>
	<td>A</td>
	<td>E</td>
    </tr>
    <xsl:for-each select="document('g2plus-index-toc.xsl')/*/xsl:template[@name='initials']/*">
      <xsl:call-template name="make-row"/>
    </xsl:for-each>
    <tr class="toc-matrix-headers">
<!--
  	<td class="toc-matrix-title-cell">
	   <a class="PSD" 
		href="javascript:showpage('../index.html',
					  'tocbrowser')"
		>ePSD</a>
-->
	<td/>
	<td>C</td>
	<td>T</td>
	<td>S</td>
	<td>A</td>
	<td>E</td>
    </tr>
  </table>
</xsl:template>

<xsl:template name="make-row">
  <tr>
    <xsl:variable name="base" select="concat('../../',$project,'/01bld/',$lang,'/')"/>
    <xsl:call-template name="make-letter-cell">
      <xsl:with-param name="letter" select="@u"/>
      <xsl:with-param name="title" select="'Browsable ePSD'"/>
    </xsl:call-template>
    <xsl:call-template name="make-cell">
      <xsl:with-param name="toc" select="concat($base,'cf.toc')"/>
      <xsl:with-param name="letter" select="@u"/>
      <xsl:with-param name="title" select="'Citation Forms'"/>
    </xsl:call-template>
    <xsl:call-template name="make-cell">
      <xsl:with-param name="toc" select="concat($base,'translit.toc')"/>
      <xsl:with-param name="letter" select="@u"/>
      <xsl:with-param name="title" select="'Transliteration'"/>
    </xsl:call-template>
    <xsl:call-template name="make-cell">
      <xsl:with-param name="toc" select="concat($base,'bases.toc')"/>
      <xsl:with-param name="letter" select="@u"/>
      <xsl:with-param name="title" select="'Bases'"/>
    </xsl:call-template>
<!--
    <xsl:call-template name="make-cell">
      <xsl:with-param name="toc" select="concat($base,'signnames.toc')"/>
      <xsl:with-param name="letter" select="@u"/>
      <xsl:with-param name="title" select="'Signnames'"/>
    </xsl:call-template>
 -->
    <xsl:call-template name="make-cell">
      <xsl:with-param name="toc" select="concat($base,'akkadian.toc')"/>
      <xsl:with-param name="letter" select="@u"/>
      <xsl:with-param name="title" select="'Akkadian'"/>
    </xsl:call-template>
    <xsl:call-template name="make-cell">
      <xsl:with-param name="toc" select="concat($base,'english.toc')"/>
      <xsl:with-param name="letter" select="@u"/>
      <xsl:with-param name="title" select="'English'"/>
    </xsl:call-template>
  </tr>
</xsl:template>

<xsl:template name="make-cell">
  <xsl:param name="toc"/>
  <xsl:param name="letter"/>
  <xsl:param name="title"/>
  <xsl:variable name="lnode" select="document($toc,$doc)//xix:letter[text() = $letter]"/>

  <td>
    <xsl:choose>
      <xsl:when test="$lnode and (not(contains($toc,'english')) or not($letter = 'Ŋ'))">

 	<xsl:variable name="js">
	  <xsl:text>'</xsl:text>
	  <xsl:value-of select="substring-before($lnode/@xl:href,'.xix')"/>
	  <xsl:text>.html'</xsl:text>
 	</xsl:variable>
        <a class="tocmatrix" 
	    href="javascript:showpage({$js}, 'tocbrowser')">
	  <span title="{$title}" class="present"><xsl:text>+</xsl:text></span>
        </a>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text> </xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </td>
</xsl:template>

<xsl:template name="make-letter-cell">
  <xsl:param name="letter"/>
  <xsl:param name="title"/>

  <xsl:variable name="lnode" 
		select="/*/xh:p[@class = 'toc-entry']/xh:a[text() = $letter]"/>

  <td>
    <xsl:choose>
      <xsl:when test="$lnode">
 	<xsl:variable name="js">
	  <xsl:text>'</xsl:text>
	  <xsl:value-of select="$lnode/xh:a/@href"/>
	  <xsl:text>'</xsl:text>
 	</xsl:variable>
        <a class="tocmatrix" 
	    href="javascript:showpage({$js}, 'tocbrowser')">
	  <span title="{$title}" class="lpresent">
	    <xsl:call-template name="html-text">
	      <xsl:with-param name="text" select="$letter"/>
	    </xsl:call-template></span>
        </a>
      </xsl:when>
      <xsl:otherwise>
        <span class="labsent">
	  <xsl:call-template name="html-text">
	    <xsl:with-param name="text" select="$letter"/>
	  </xsl:call-template></span>
      </xsl:otherwise>
    </xsl:choose>
  </td>
</xsl:template>

</xsl:stylesheet>
