<?xml version='1.0'?>

<!--

 XSL Stylesheet to produce XHTML version of XTF texts.

 Steve Tinney 12/22/05, for the ORACC.

 v1.1.  Placed in the Public Domain.

-->

<xsl:stylesheet version="1.0" 
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:lem="http://oracc.org/ns/lemma/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:exsl="http://exslt.org/common"
  xmlns:md="http://oracc.org/ns/xmd/1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  exclude-result-prefixes="xcl xtf lem md"
  extension-element-prefixes="exsl">

<xsl:variable name="quote-char"><xsl:text>'</xsl:text></xsl:variable>
<xsl:variable name="escaped-quote"><xsl:text>\'</xsl:text></xsl:variable>
<xsl:template name="escape-quotes">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="contains($text,$quote-char)">
      <xsl:variable name="pre" select="substring-before($text,$quote-char)"/>
      <xsl:variable name="post">
	<xsl:call-template name="escape-quotes">
	  <xsl:with-param name="text" 
		select="substring-after($text,concat($pre,$quote-char))"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($pre,$escaped-quote,$post)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sub">
  <xsl:param name="text"/>
  <xsl:param name="render"/>
  <xsl:choose>
    <xsl:when test="$render='XHTML'">
      <sub>
	<xsl:value-of select="$text"/>
      </sub>
    </xsl:when>
<!--
    <xsl:when test="$render='FO'">
      <fo:inline vertical-align="sub">
	<xsl:value-of select="$text"/>
      </fo:inline>
    </xsl:when>
    <xsl:when test="$render='ATF'">
      <xsl:value-of select="$text"/>
    </xsl:when>
 -->
    <xsl:otherwise>
      <xsl:message>text-util.xsl:sub: unhandled render type '<xsl:value-of 
      	select="$render"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sup">
  <xsl:param name="text"/>
  <xsl:param name="render"/>
  <xsl:choose>
    <xsl:when test="$render='XHTML'">
      <sup>
	<xsl:call-template name="html-text">
  	  <xsl:with-param name="text" select="$text"/>
	</xsl:call-template>
      </sup>
    </xsl:when>
    <!--
    <xsl:when test="$render='FO'">
      <fo:inline vertical-align="super">
	<xsl:call-template name="fo-text">
  	  <xsl:with-param name="text" select="$text"/>
	</xsl:call-template>
      </fo:inline>
    </xsl:when>
    <xsl:when test="$render='ATF'">
      <xsl:text>{</xsl:text>
	<xsl:call-template name="atf-text">
  	  <xsl:with-param name="text" select="$text"/>
	</xsl:call-template>
      <xsl:text>}</xsl:text>
    </xsl:when>
    -->
    <xsl:otherwise>
      <xsl:message>text-util.xsl:sup: unhandled render type '<xsl:value-of 
	select="$render"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:variable name="xtf1-utf8" select="'ṣṢṭṬŋŊ'"/>
<xsl:variable name="xtf1-html" select="'şŞţŢĝĜ'"/>

<xsl:template name="xtf1-html-translate">
  <xsl:param name="t"/>
  <xsl:value-of select="translate($t,$xtf1-utf8,$xtf1-html)"/>
</xsl:template>

<xsl:output method="xml" indent="no" encoding="utf-8"/>
<xsl:strip-space elements="*"/>

<xsl:key name="wid" match="xcl:l" use="@ref"/>

<xsl:variable name="PQ-id">
  <xsl:choose>
    <xsl:when test="starts-with(/*/@xml:id,'s.')">
      <xsl:value-of select="substring-after(/*/@xml:id,'s.')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="/*/@xml:id"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name="xcl" select="concat($PQ-id,'.xcl')"/>
<xsl:variable name="xmd" select="concat($PQ-id,'.xmd')"/>

<xsl:variable name="pubimg">
  <xsl:if test="document($xmd,/)/*/md:public_images='yes'">
    <xsl:choose>
      <xsl:when test="/*/md:images/md:img[@type='p']">
<!--      <xsl:when test="string-length(document($xmd,/)/*/md:photo_up) > 0">-->
        <xsl:text>**</xsl:text>
      </xsl:when>
      <xsl:when test="/*/md:images/md:img[@type='l']">
<!--      <xsl:when test="string-length(document($xmd,/)/*/md:lineart_up) > 0">-->
        <xsl:text>*</xsl:text>
      </xsl:when>
      <xsl:otherwise/>
    </xsl:choose>
  </xsl:if>
</xsl:variable>

<xsl:template match="/">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template name="base-attr">
  <xsl:copy-of select="@n|@xml:id"/>
  <xsl:attribute name="xlabel">
    <xsl:value-of select="concat($pubimg,@n)"/>
  </xsl:attribute>
</xsl:template>

<xsl:template match="xtf:transliteration">
  <table class="transliteration">
    <xsl:call-template name="base-attr"/>
    <xsl:apply-templates/>
  </table>
</xsl:template>

<xsl:template match="xtf:refunit">
  <xsl:apply-templates select="*"/>
</xsl:template>

<xsl:template match="xtf:composite">
  <table class="composite">
    <xsl:call-template name="base-attr"/>
    <xsl:apply-templates/>
  </table>
</xsl:template>

<xsl:template match="xtf:score">
  <xsl:variable name="pid" select="substring-after(/*/@xml:id,'s.')"/>
  <score n="{@n}">
    <xsl:for-each select="xtf:lg">
      <table block-id="{@n}" class="score-block">
	<xsl:call-template name="base-attr"/>
	<xsl:apply-templates/>
      </table>
    </xsl:for-each>
  </score>
</xsl:template>

<xsl:template match="xtf:m">
  <tr class="h1">
    <td colspan="10">
      <span class="div"><xsl:value-of 
           select="concat('(',@subtype|@division|text(),')')"/></span>
    </td>
  </tr>
</xsl:template>

<xsl:template match="xtf:div">
  <tr class="h1">
    <td colspan="10">
      <xsl:variable name="itype">
	<xsl:call-template name="init-cap">
 	  <xsl:with-param name="str" select="@subtype|@type"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:variable name="n">
	<xsl:if test="@n">
	  <xsl:value-of select="concat(' ',@n)"/>
	</xsl:if>
      </xsl:variable>
      <span class="div"><xsl:value-of select="concat($itype,$n)"/></span>
    </td>
  </tr>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:object">
  <xsl:if test="not(@type='tablet') or @n">
    <tr class="h">
      <td colspan="10">
	<span class="h2">
	  <xsl:variable name="object">
	    <xsl:choose>
	      <xsl:when test="@type='other'">
		<xsl:value-of select="@object"/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="@type"/>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:variable>	
	  <xsl:if test="not($object='tablet')">
	    <xsl:call-template name="init-cap">
              <xsl:with-param name="str" select="$object"/>
            </xsl:call-template>
	    <xsl:text> </xsl:text>
	  </xsl:if>
	  <xsl:choose>
	    <xsl:when test="@n">
	       <xsl:value-of select="@n"/>
	    </xsl:when>
            <xsl:when test="@face">
              <xsl:text> </xsl:text>
              <xsl:value-of select="@face"/>
            </xsl:when>
	    <xsl:when test="@type='other'"/>
	    <xsl:otherwise>
              <xsl:call-template name="init-cap">
		<xsl:with-param name="str" select="@type"/>
              </xsl:call-template>
            </xsl:otherwise>
	  </xsl:choose>    
	  <xsl:if test="@certain='n'">
            <xsl:text>?</xsl:text>
	  </xsl:if>
	</span>
      </td>
    </tr>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="xtf:l|xtf:lg/xtf:l">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xtf:surface|xtf:sealing">
  <xsl:if test="ancestor::xtf:object[@type='tablet'] 
		or @face
		or @surface
		or self::xtf:sealing">
    <tr class="h">
      <td colspan="10">
	<span class="h2">
	  <xsl:if test="not(local-name()='surface')">
	    <xsl:call-template name="init-cap">
              <xsl:with-param name="str" select="local-name()"/>
            </xsl:call-template>
	    <xsl:text> </xsl:text>
	  </xsl:if>
	  <xsl:choose>
	    <xsl:when test="@n">
	      <xsl:value-of select="@n"/>
	    </xsl:when>
            <xsl:when test="@face">
              <xsl:text> </xsl:text>
              <xsl:value-of select="@face"/>
            </xsl:when>
	    <xsl:otherwise>
	      <xsl:choose>
		<xsl:when test="@type='obverse' or @type='reverse'">
		  <xsl:if test="ancestor::xtf:object[@type='tablet']">
		    <xsl:call-template name="init-cap">
		      <xsl:with-param name="str" select="@type"/>
		    </xsl:call-template>
		  </xsl:if>
		</xsl:when>
		<xsl:when test="@type='surface'">
		  <xsl:call-template name="init-cap">
		    <xsl:with-param name="str" select="@surface"/>
		  </xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:call-template name="init-cap">
		    <xsl:with-param name="str" select="@type"/>
		  </xsl:call-template>
		</xsl:otherwise>
	      </xsl:choose>
            </xsl:otherwise>
	  </xsl:choose>    
	  <xsl:if test="@certain='n'">
            <xsl:text>?</xsl:text>
	  </xsl:if>
	</span>
      </td>
    </tr>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="xtf:l|xtf:lg/xtf:l">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xtf:column">
  <xsl:if test="not(@n = '0')">
    <tr class="h">
      <td colspan="10">
	<span class="h2">
	  <xsl:call-template name="init-cap">
            <xsl:with-param name="str" select="local-name()"/>
          </xsl:call-template>
	  <xsl:text> </xsl:text>
	  <xsl:variable name="n" 
			select="document('label-info.xml')/*/*[@n=current()/@n]/@a"/>
	  <xsl:choose>
	    <xsl:when test="string-length($n) > 0">
	      <xsl:value-of select="$n"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="@n"/>
	    </xsl:otherwise>
	  </xsl:choose>
	  <xsl:if test="@primes"><xsl:value-of select="@primes"/></xsl:if>
	  <xsl:if test="@remark='y'"><xsl:text>!</xsl:text></xsl:if>
	  <xsl:if test="@certain='n'"><xsl:text>?</xsl:text></xsl:if>
	</span>
      </td>
    </tr>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="xtf:l|xtf:lg/xtf:l">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xtf:lg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:noncolumn">
  <tr class="noncolumn">
    <td colspan="10"><xsl:call-template name="do-non-c-or-l"/></td>
  </tr>
</xsl:template>

<xsl:template match="xtf:nonl">
  <tr class="nonl">
    <td class="nonlnum">&#xa0;</td>
    <td colspan="10" class="nonlbody"><xsl:call-template name="do-non-c-or-l"/></td>
  </tr>
</xsl:template>

<xsl:template name="do-non-c-or-l">
  <span class="noncl">
    <xsl:text> (</xsl:text>
    <xsl:if test="@type='traces'">
      <xsl:text>traces</xsl:text>
      <xsl:if test="@extent"><xsl:text> of </xsl:text></xsl:if>
    </xsl:if>
    <xsl:if test="@extent">
      <xsl:value-of select="@extent"/>
      <xsl:text> </xsl:text>
      <xsl:if test="@extent = 'rest'">
        <xsl:text>of </xsl:text>
      </xsl:if>
    </xsl:if>
    <xsl:if test="not(@type='seal') and not(@type='ruling') and @extent">
      <xsl:choose>
        <xsl:when test="local-name() = 'nonl'">
          <xsl:text>line</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:text>column</xsl:text>
        </xsl:otherwise>
      </xsl:choose>
      <xsl:if test="@extent > 1"><xsl:text>s</xsl:text></xsl:if>
      <xsl:if test="not(@type='traces')"><xsl:text> </xsl:text></xsl:if>
    </xsl:if>
    <xsl:if test="not(@type='traces')">
      <xsl:value-of select="@type"/>
    </xsl:if>
    <xsl:if test="@ref">
      <xsl:text> </xsl:text>
      <xsl:value-of select="@ref"/>
    </xsl:if>
    <xsl:text>)</xsl:text>
  </span>
</xsl:template>

<xsl:template match="xtf:nong">
  <span class="nong">
    <xsl:choose>
      <xsl:when test="@type='broken'">
<!--
        <xsl:text> (</xsl:text>
        <xsl:value-of select="@extent"/>
        <xsl:text> signs broken)</xsl:text>
-->
         <xsl:text> [...] </xsl:text>
      </xsl:when>
      <xsl:when test="@type='maybe-broken'">
         <xsl:text> [(...)] </xsl:text>
      </xsl:when>
      <xsl:when test="@type='traces'">
        <xsl:text> (</xsl:text>
        <xsl:value-of select="@type"/>
        <xsl:text> of </xsl:text>
        <xsl:value-of select="@extent"/>
        <xsl:text> signs)</xsl:text>
      </xsl:when>
      <!--
	FIXME: does maybe-traces need to do the same extent handling as traces?
        -->
      <xsl:when test="@type='maybe-traces'">
        <xsl:text> (...) </xsl:text>
      </xsl:when>
      <xsl:when test="@type='gap'">
        <xsl:text> (</xsl:text>
        <xsl:value-of select="@type"/>
        <xsl:text> of </xsl:text>
        <xsl:value-of select="@extent"/>
        <xsl:text> signs)</xsl:text>
      </xsl:when>
      <xsl:when test="@type='other'">
        <xsl:text> (</xsl:text>
	<xsl:value-of select="text()"/>
        <xsl:text>)</xsl:text>
      </xsl:when>
      <xsl:when test="@type='seal'">
        <xsl:text> (seal </xsl:text>
        <xsl:value-of select="@ref"/>
        <xsl:text>)</xsl:text>
      </xsl:when>
      <xsl:when test="@type='newline'">
        <xsl:text>;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:message><xsl:value-of 
		select="/*/@xml:id"/>: NONG type not handled: <xsl:value-of 
			select="@type"/></xsl:message>
      </xsl:otherwise>
    </xsl:choose>
  </span>
</xsl:template>

<xsl:template match="xtf:l">
  <xsl:variable name="href">
    <xsl:choose>
      <xsl:when test="/xtf:composite">
	<xsl:value-of select="/*/@xml:id"/>
	<xsl:text>_</xsl:text>
	<xsl:value-of select="@n"/>
	<xsl:text>.html</xsl:text>
      </xsl:when>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="name">
    <xsl:if test="/xtf:transliteration">
      <xsl:value-of select="@xml:id"/>
    </xsl:if>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="xtf:mpx">
      <xsl:call-template name="format-line">
	<xsl:with-param name="lnode" select="xtf:mpx[1]"/>
   	<xsl:with-param name="href" select="$href"/>
   	<xsl:with-param name="name" select="$name"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="format-line">
	<xsl:with-param name="lnode" select="."/>
   	<xsl:with-param name="href" select="$href"/>
   	<xsl:with-param name="name" select="$name"/>
      </xsl:call-template>
      <xsl:apply-templates mode="after-line" select="xtf:note"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="format-line">
  <xsl:param name="href"/>
  <xsl:param name="lnode"/>
  <xsl:param name="name"/>
  <xsl:for-each select="$lnode">
  <tr class="l">
    <!--NOTE: XHTML 1.0 DOES NOT USE XML:ID, this needs to be hacked on output
	for XHTML pages, but we keep to XML:ID here because the Context Engine
	uses the TXH files to return line context-->
    <xsl:variable name="lid">
      <xsl:choose>
	<xsl:when test="self::xtf:mpx">
	  <xsl:value-of select="../@xml:id"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="@xml:id"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="label">
      <xsl:choose>
	<xsl:when test="self::xtf:mpx">
	  <xsl:value-of select="../@label"/>
	</xsl:when>
	<xsl:when test="not(@label)">
	  <xsl:value-of select="@n"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="@label"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="lnum">
      <xsl:choose>
	<xsl:when test="self::xtf:mpx">
	  <xsl:value-of select="../@n"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="@n"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:attribute name="xml:id">
      <xsl:value-of select="$lid"/>
    </xsl:attribute>
    <xsl:choose>
      <xsl:when test="string-length($href) > 0">
	<td class="lnum">
	  <span class="xlabel">
<!--	    <xsl:value-of select="concat($pubimg,'(',/*/@n,' ',$label,')')"/> -->
	    <xsl:value-of select="$label"/>
	  </span>
	  <span class="lnum">
	    <a href="javascript:showscore('{/*/@xml:id}','{$label}')"
	       name="{concat('a.',@xml:id)}">
              <xsl:value-of select="@n"/><xsl:text>.</xsl:text>
            </a>
	  </span>
	</td>
 	<xsl:choose>
	  <xsl:when test="xtf:c">
 	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
 	    <td><xsl:apply-templates/></td>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
	<!-- FIXME: is this block necessary? An old version had a bug where $name
	could never be > 0, so perhaps the href block above was my way of "fixing" that? -->
      <xsl:when test="string-length($name) > 0">
	<td class="lnum">
	  <span class="xlabel">
<!--	    <xsl:value-of select="concat($pubimg,'(',/*/@n,' ',$label,')')"/>-->
	    <xsl:value-of select="$label"/>
	  </span>
	  <span class="lnum"><a name="{$name}">
          <xsl:value-of select="$lnum"/><xsl:text>.</xsl:text>
        </a></span></td>
 	<xsl:choose>
	  <xsl:when test="xtf:c">
 	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
 	    <td><xsl:apply-templates/></td>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="/xtf:score">
        <td class="lnum"><xsl:value-of select="$lnum"/><xsl:text>.</xsl:text></td>
 	<xsl:choose>
	  <xsl:when test="xtf:c">
 	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
 	    <td><xsl:apply-templates/></td>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <td class="lnum"><xsl:value-of select="$lnum"/><xsl:text>.</xsl:text></td>
 	<xsl:choose>
	  <xsl:when test="xtf:c">
 	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
 	    <td><xsl:apply-templates/></td>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </tr>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xtf:mpx"/>

<xsl:template match="xtf:e">
  <xsl:variable name="href">
    <xsl:choose>
      <xsl:when test="/xtf:score">
	<xsl:value-of select="@p"/>
	<xsl:text>.html#</xsl:text>
	<xsl:value-of select="@plid"/>
      </xsl:when>
    </xsl:choose>
  </xsl:variable>
  <tr class="e">
    <xsl:choose>
      <xsl:when test="string-length($href) > 0">
        <td> </td>
        <xsl:choose>
          <xsl:when test="xtf:c">
            <xsl:apply-templates/>
          </xsl:when>
          <xsl:otherwise>
            <td><xsl:apply-templates/></td>
          </xsl:otherwise>
        </xsl:choose>
        <td><span class="enum"><a href="javascript:showexemplar('{@p}','{@hlid}')">
               <xsl:value-of select="concat(@n,' ',@l)"/>
            </a></span>
        </td>
      </xsl:when>
      <xsl:otherwise>
        <td><span class="enum"><xsl:value-of select="concat(@n,' ',@l)"
	  	/><xsl:text>. </xsl:text></span></td>	
        <xsl:choose>
    	  <xsl:when test="xtf:c">
            <xsl:apply-templates/>
 	  </xsl:when>
	  <xsl:otherwise>
            <td><xsl:apply-templates/></td>
	  </xsl:otherwise>
        </xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </tr>
</xsl:template>

<xsl:template name="format-e">
</xsl:template>

<xsl:template match="xtf:n">
  <span class="n"><xsl:apply-templates/></span>
  <xsl:if test="following-sibling::*">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="xtf:c">
  <td class="c"><xsl:apply-templates/></td>
</xsl:template>

<xsl:template match="xtf:f">
  <span class="{@type}">
    <xsl:apply-templates/>
  </span>
  <xsl:if test="following-sibling::*">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="xtf:w">
  <xsl:variable name="form">
    <xsl:for-each select="*">
      <xsl:apply-templates mode="form" select="."/>
      <xsl:call-template name="maybe-hyphen"/>
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="lang" select="ancestor::*[@xml:lang][1]/@xml:lang"/>
  <xsl:variable name="rws" select="ancestor::*[@rws][1]/@rws"/>
<!--  <span class="w" form="{$form}" id="{@xml:id}" xml:lang="{$lang}" rws="{$rws}">-->
  <span class="w" id="{@xml:id}">
    <xsl:variable name="wid" select="@xml:id"/>
    <xsl:variable name="cfgw">
      <xsl:for-each select="document($xcl,/)">
	<xsl:variable name="node" select="key('wid',$wid)[1]"/>
	<xsl:choose>
	  <xsl:when test="$node/dsu">
	    <xsl:call-template name="escape-quotes">
	      <xsl:with-param name="text" select="$node/dsu[1]/@term"/>
	    </xsl:call-template>
<!--
	    <xsl:value-of select="$node/dsu[1]/@term"/>
 -->
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:if test="contains($node/@cfgw,'[') 
			and not($node/@cfgw = '[]')">
	      <xsl:call-template name="escape-quotes">
	        <xsl:with-param name="text" select="$node/@cfgw"/>
	      </xsl:call-template>
<!--
	      <xsl:value-of select="$node/@cfgw"/>
 -->
 	    </xsl:if>
	  </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
    </xsl:variable>
    <xsl:variable name="pos">
      <xsl:for-each select="document($xcl,/)">
	<xsl:value-of select="key('wid',$wid)[1]/@pos"/>
      </xsl:for-each>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="string-length($cfgw) > 0">
	<xsl:variable name="hcfgw">
	  <xsl:call-template name="xtf1-html-translate">
	    <xsl:with-param name="t" select="$cfgw"/>
	  </xsl:call-template>
	</xsl:variable>
  	<a href="javascript:popepsd('{$cfgw}','{$wid}')"
	   title="{$hcfgw}">
          <xsl:for-each select="*">
            <xsl:apply-templates select="."/>
            <xsl:call-template name="maybe-hyphen"/>
          </xsl:for-each>
 	</a>
      </xsl:when>
      <xsl:when test="string-length($pos) > 0">
	<xsl:attribute name="title"><xsl:value-of select="$pos"/></xsl:attribute>
        <xsl:for-each select="*">
          <xsl:apply-templates select="."/>
          <xsl:call-template name="maybe-hyphen"/>
        </xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
        <xsl:for-each select="*">
          <xsl:apply-templates select="."/>
          <xsl:call-template name="maybe-hyphen"/>
        </xsl:for-each>
      </xsl:otherwise>
    </xsl:choose>
  </span>
  <xsl:if test="following-sibling::*">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="maybe-hyphen">
  <xsl:choose>
    <xsl:when test="@gloss='cont' 
		    or (@gloss='post' and following::xtf:g[1]/@gloss='cont')">
<!--      <sup><xsl:text>-</xsl:text></sup> -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="not(position() = last())
		and not(@gloss = 'pre')
	        and not(following::xtf:g[1]/@gloss='post')
	        and not(following::*[1][local-name()='gloss'])">
        <xsl:text>-</xsl:text>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="form" match="xtf:g">
  <xsl:if test="not(@sign='ed.removed')">
    <xsl:if test="@gloss"><xsl:text>{</xsl:text></xsl:if>
    <xsl:choose>
      <xsl:when test="@role='logogram'">
	<xsl:call-template name="render-grapheme">
          <xsl:with-param name="g" select="."/>
          <xsl:with-param name="c" select="'logogram'"/>
	</xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="render-grapheme">
          <xsl:with-param name="g" select="."/>
          <xsl:with-param name="c" select="@nametype"/>
	</xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="@gloss"><xsl:text>}</xsl:text></xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template match="xtf:gg">
  <xsl:for-each select="*">
    <xsl:apply-templates select="."/>
    <xsl:if test="not(position() = last())
		and not(@gloss='pre')
		and not(following-sibling::*[1][@gloss='post'])">
      <xsl:text>.</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template mode="form" match="xtf:cg|xtf:gg|xtf:nong|xtf:cg.g|xtf:cg.gg">
  <xsl:apply-templates select="."/>
</xsl:template>

<xsl:template mode="form" match="xtf:igg">
  <xsl:apply-templates mode="form" select="*[1]"/>
</xsl:template>

<xsl:template mode="form" match="xtf:gloss" />

<xsl:template match="xtf:igg">
  <xsl:apply-templates mode="igg-interp" select="*[1]"/>

  <xsl:text>(</xsl:text>
    <xsl:apply-templates mode="igg-verbatim" select="*[2]"/>
  <xsl:text>)</xsl:text>

  <xsl:for-each select="descendant::xtf:g[position()=last()]">
    <xsl:variable name="prev-g" 
	        select="preceding::xtf:g[1][ancestor::xtf:l/@xml:id 
					  = current()/ancestor::xtf:l/@xml:id]"/>
    <xsl:variable name="next-g" 
	        select="following::xtf:g[1][ancestor::xtf:l/@xml:id 
					  = current()/ancestor::xtf:l/@xml:id]"/>
    <xsl:call-template name="close-sign">
      <xsl:with-param name="next-g" select="$next-g"/>
    </xsl:call-template>
    <xsl:call-template name="close-breakage">
      <xsl:with-param name="next-g" select="$next-g"/>
    </xsl:call-template>
  </xsl:for-each>
</xsl:template>

<xsl:template mode="igg-interp" match="xtf:gg">
  <xsl:for-each select="*">
    <xsl:apply-templates select="."/>
    <xsl:if test="not(position() = last())">
      <xsl:text>:</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template mode="igg-verbatim" match="xtf:gg">
  <xsl:for-each select="*">
    <xsl:apply-templates select="."/>
    <xsl:if test="not(position() = last())">
      <xsl:text>&#xa0;</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template mode="igg-verbatim" match="xtf:cg">
  <xsl:for-each select="*">
    <xsl:apply-templates select="."/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xtf:cg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:cg.g">
  <xsl:call-template name="render-grapheme">
    <xsl:with-param name="g" select="."/>
    <xsl:with-param name="c" select="'signref'"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="xtf:cg.gg">
  <xsl:text>(</xsl:text>
  <xsl:for-each select="*">
    <xsl:apply-templates select="."/>
<!--
    <xsl:if test="not(position() = last())">
      <xsl:text>.</xsl:text>
    </xsl:if>
 -->
  </xsl:for-each>
  <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="xtf:cg.rel">
  <xsl:choose>
    <xsl:when test="@c='adjacent'">
      <xsl:text>.</xsl:text>
    </xsl:when>
    <xsl:when test="@c='ligatured'">
      <xsl:text>+</xsl:text>
    </xsl:when>
    <xsl:when test="@c='times'">
      <xsl:text>×</xsl:text>
    </xsl:when>
    <xsl:when test="@c='over'">
      <xsl:text>&amp;</xsl:text>
    </xsl:when>
    <xsl:when test="@c='opposed'">
      <xsl:text>@</xsl:text>
    </xsl:when>
    <xsl:when test="@c='crossed'">
      <xsl:text>%</xsl:text>
    </xsl:when>
    <xsl:when test="@c='or'">
      <xsl:text>/</xsl:text>
    </xsl:when>
    <xsl:when test="@c='exorder'">
      <xsl:text>:</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>unknown cg.rel type '<xsl:value-of select="@c"/>'</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="igg-interp" match="xtf:g|xtf:cg.g">
  <xsl:apply-templates select="."/>
</xsl:template>

<xsl:template mode="igg-verbatim" match="xtf:g|xtf:cg.g">
  <xsl:apply-templates select=".">
    <xsl:with-param name="igg-verb" select="'yes'"/>
  </xsl:apply-templates>
</xsl:template>

<xsl:template match="xtf:gloss">
  <sup class="gloss">
    <xsl:apply-templates/>
  </sup>
</xsl:template>

<xsl:template match="xtf:g">
  <xsl:param name="igg-verb"/>
  <xsl:choose>
    <xsl:when test="@gloss">
      <sup>
<!--	<xsl:if test="@gloss='post'">.</xsl:if> -->
	<xsl:if test="@gloss='post' 
		and preceding-sibling::*[1][local-name() = 'gloss']">
	  <xsl:text> </xsl:text>
        </xsl:if>
	<xsl:if test="@gloss='cont' 
			and preceding-sibling::xtf:g[@gloss='post']"
		><xsl:text>-</xsl:text></xsl:if>
        <xsl:call-template name="xtf1-format-g"/>
	<xsl:if test="@gloss='cont' and following-sibling::xtf:g[@gloss='pre']"
		><xsl:text>-</xsl:text></xsl:if>
<!--	<xsl:if test="@gloss='pre'">.</xsl:if> -->
      </sup>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="xtf1-format-g">
        <xsl:with-param name="igg-verb" select="$igg-verb"/>
      </xsl:call-template>
      <xsl:if test="(local-name(..)='l') 
	and not(position()=last())">
        <xsl:text> </xsl:text>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="xtf1-format-g">
  <xsl:param name="igg-verb"/>
  <xsl:variable name="prev-g" 
      select="preceding::xtf:g[1][ancestor::xtf:l/@xml:id = current()/ancestor::xtf:l/@xml:id]"/>
  <xsl:variable name="next-g" 
      select="following::xtf:g[1][ancestor::xtf:l/@xml:id = current()/ancestor::xtf:l/@xml:id]"/>

  <xsl:if test="not($igg-verb)">
    <xsl:call-template name="open-breakage">
      <xsl:with-param name="prev-g" select="$prev-g"/>
    </xsl:call-template>
    <xsl:call-template name="open-sign">
      <xsl:with-param name="prev-g" select="$prev-g"/>
    </xsl:call-template>
  </xsl:if>

  <xsl:choose>
    <xsl:when test="lang('akk') and not(@nametype='signref') and not(@role='logogram')">
      <i>
        <xsl:call-template name="render-grapheme">
          <xsl:with-param name="g" select="."/>
        </xsl:call-template>
      </i>
    </xsl:when>
    <xsl:when test="@role='logogram'">
      <xsl:call-template name="render-grapheme">
        <xsl:with-param name="g" select="."/>
        <xsl:with-param name="c" select="'logogram'"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="render-grapheme">
        <xsl:with-param name="g" select="."/>
        <xsl:with-param name="c" select="@nametype"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>

  <xsl:if test="@collated='y'
		or @sign='ed.emended'
		or @sign='unusual.form'
		or @uncertain='y'">
    <sup>
      <xsl:if test="@collated='y'"><xsl:text>*</xsl:text></xsl:if>
      <xsl:if test="@sign='ed.emended'"><xsl:text>!</xsl:text></xsl:if>
      <xsl:if test="@sign='unusual.form'"><xsl:text>!</xsl:text></xsl:if>
      <xsl:if test="@uncertain='y'"><xsl:text>?</xsl:text></xsl:if>
    </sup>
  </xsl:if>

  <xsl:if test="not($igg-verb)">
    <xsl:call-template name="close-breakage">
      <xsl:with-param name="next-g" select="$next-g"/>
    </xsl:call-template>
    <xsl:call-template name="close-sign">
      <xsl:with-param name="next-g" select="$next-g"/>
    </xsl:call-template>
  </xsl:if>

</xsl:template>

<xsl:template match="xtf:cmt"/>

<xsl:template match="xtf:*">
  <xsl:variable name="loc" 
	select="ancestor::*[@xml:id][1]/@xml:id"/>
  <xsl:message>xtf2txh.xsl:<xsl:value-of select="$loc"
	/> untrapped element <xsl:value-of 
     select="local-name()"/></xsl:message>
</xsl:template>

<xsl:template name="init-cap">
  <xsl:param name="str"/>
  <xsl:value-of select="translate(substring($str,1,1),$lc,$uc)"/>
  <xsl:value-of select="substring($str,2)"/>
</xsl:template>

<xsl:template match="xtf:note">
  <xsl:choose>
    <xsl:when test="ancestor::xtf:l"/>
    <xsl:otherwise>
      <tr>
	<td/>
	<td colspan="10">
	  <xsl:call-template name="format-note"/>
	</td>
      </tr>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template mode="after-line" match="xtf:note">
  <tr>
    <td/>
    <td colspan="10">
      <span class="note">
        <xsl:text>(</xsl:text>
          <xsl:apply-templates/>
        <xsl:text>)</xsl:text>
      </span>
    </td>
  </tr>
</xsl:template>

<xsl:template name="format-note">
<!--  <xsl:text>(</xsl:text>
  <span class="note">NOTE: </span>
 -->
  <span class="note"><xsl:apply-templates/></span>
<!--
  <xsl:text>)</xsl:text>
 -->
</xsl:template>

<!-- The content model of S has to be reviewed before it's worth
 putting any real effort into this template -->
<xsl:template match="xtf:s">
  <xsl:choose>
    <xsl:when test="ancestor::xtf:l">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <tr>
	<xsl:apply-templates/>
      </tr>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!-- 
  Depending on the layout of notes in ETCSL it is probably enough just
  to apply-templates here.
 -->
<xsl:template match="xtf:lvg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:wvg">
  <xsl:text> {</xsl:text>
  <xsl:for-each select="*">
    <xsl:apply-templates/>
    <xsl:if test="not(position()=last())">
      <xsl:text>; </xsl:text>
    </xsl:if>
  </xsl:for-each>
  <xsl:text>} </xsl:text>
</xsl:template>

<!--

 XSLT function: render-grapheme

 This function properly renders graphemes in the XTF or GDL 
 vocabularies into either XHTML or XSL-FO.  See xtf-HTML.xsl
 for examples of how to call this function in your code.

 Parameters:

	g : the grapheme to render, as a text node
	c : the class of the grapheme
  	render : the type of rendering to perform; defaults to 
		XHTML; set to 'FO' for XSL-FO output; set to ATF 
		for ATF-compliant ASCII output.

 Steve Tinney, 1/5/2006.  Placed in the public domain.

 -->

<xsl:variable name="subdig" select="'₀₁₂₃₄₅₆₇₈₉₊'"/>
<xsl:variable name="regdig" select="'0123456789x~'"/>

<xsl:template name="render-grapheme">
  <xsl:param name="g"/>
  <xsl:param name="c" select="''"/>
  <xsl:param name="render" select="'XHTML'"/>
 
  <xsl:variable name="is-num-grapheme" select="contains($g,'(')"/>
  <!-- if this is a number grapheme, render the prefix right away
 	and remember that we need to dump a ')' after the subdigits -->
  <xsl:if test="$is-num-grapheme">
    <xsl:value-of select="substring-before($g,'(')"/>
    <xsl:text>(</xsl:text>
    <xsl:call-template name="render-grapheme">
      <xsl:with-param name="g" 
	 	select="substring-before(substring-after($g,'('),')')"/>
      <xsl:with-param name="render" select="$render"/>
    </xsl:call-template>
    <xsl:text>)</xsl:text>
  </xsl:if>

  <!-- render body -->
  <xsl:variable name="body-sans-mods">
    <xsl:choose>
      <xsl:when test="$is-num-grapheme">
	<xsl:text/>
      </xsl:when>
      <xsl:when test="contains($g,'~') and contains($g,'@')">
	<xsl:choose>
  	  <xsl:when test="string-length(substring-before('~',$g))
			  &lt; string-length(substring-before('@',$g))">
	    <xsl:value-of select="substring-before($g,'~')"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="substring-before($g,'@')"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="contains($g,'~')">
	<xsl:value-of select="substring-before($g,'~')"/>
      </xsl:when>
      <xsl:when test="contains($g,'@')">
	<xsl:value-of select="substring-before($g,'@')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$g"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="body-with-digits">
    <xsl:choose>
      <xsl:when test="$is-num-grapheme">
	<xsl:value-of select="substring-before(substring-after($body-sans-mods,'('),
					       ')')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$body-sans-mods"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="body" select="translate($body-with-digits,$subdig,'')"/>

  <xsl:choose>
    <xsl:when test="$render='XHTML'">
      <xsl:choose>
        <xsl:when test="$c = 'signref'">
          <span class="sign">
<!--
	    <xsl:call-template name="html-text">
  	      <xsl:with-param name="text" select="translate($body,$lc,$uc)"/>
	    </xsl:call-template>
 -->
	    <xsl:value-of select="translate($body,$lc,$uc)"/>
          </span>
        </xsl:when>
        <xsl:when test="$c = 'logogram'">
          <span class="logogram">
<!--
	    <xsl:call-template name="html-text">
	      <xsl:with-param name="text" select="translate($body,$lc,$uc)"/>
   	    </xsl:call-template>
 -->
	    <xsl:value-of select="translate($body,$lc,$uc)"/>
          </span>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="html-translate">
	    <xsl:with-param name="t" select="$body"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
<!--
    <xsl:when test="$render='FO'">
     <xsl:choose>
        <xsl:when test="$c = 'signref'">
          <fo:inline-wrapper font-style="normal">
	    <xsl:call-template name="fo-text">
  	      <xsl:with-param name="text" select="translate($body,$lc,$uc)"/>
	    </xsl:call-template>
          </fo:inline-wrapper>
        </xsl:when>
        <xsl:when test="$c = 'logogram'">
          <fo:inline-wrapper font-style="normal" font-size="80%">
	    <xsl:call-template name="fo-text">
	      <xsl:with-param name="text" select="translate($body,$lc,$uc)"/>
   	    </xsl:call-template>
          </fo:inline-wrapper>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="fo-text">
	    <xsl:with-param name="text" select="$body"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="$render='ATF'">
     <xsl:choose>
        <xsl:when test="$c = 'signref'">
	  <xsl:call-template name="atf-text">
  	    <xsl:with-param name="text" select="translate($body,$lc,$uc)"/>
	  </xsl:call-template>
        </xsl:when>
        <xsl:when test="$c = 'logogram'">
	  <xsl:call-template name="atf-text">
	    <xsl:with-param name="text" select="translate($body,$lc,$uc)"/>
   	  </xsl:call-template>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="atf-text">
	    <xsl:with-param name="text" select="$body"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
 -->
    <xsl:otherwise>
      <xsl:message>render-g: unhandled render type <xsl:value-of select="$render"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>

  <xsl:if test="string-length($body-sans-mods) > 0">
    <!-- render subscript -->
    <xsl:variable name="sub" select="substring($body-with-digits,
		  			       string-length($body)+1)"/>
    <xsl:if test="string-length($sub)>0">
      <xsl:call-template name="sub">
        <xsl:with-param name="text" select="translate($sub,$subdig,$regdig)"/>
        <xsl:with-param name="render" select="$render"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:if>

  <!-- render post-body stuff: @g, @g~a and @g~a@g are all possible;
	~a@g is not possible without an @-mod just before it -->

  <xsl:variable name="rest">
    <xsl:choose>
      <xsl:when test="$is-num-grapheme">
	<xsl:value-of select="substring-after($g,')')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="substring-after($g,$body-sans-mods)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="contains($rest,'~')">
      <xsl:value-of select="substring-before($rest,'~')"/>
      <xsl:variable name="var" select="substring-after($rest,'~')"/>
      <xsl:choose>
  	<xsl:when test="contains($var,'@')">
  	  <xsl:call-template name="sub">
	    <xsl:with-param name="text" select="substring-before($var,'@')"/>
	    <xsl:with-param name="render" select="$render"/>
          </xsl:call-template>
  	  <xsl:text>@</xsl:text>
  	  <xsl:value-of select="substring-after($var,'@')"/>
  	</xsl:when>
  	<xsl:otherwise>
  	  <xsl:call-template name="sub">
	    <xsl:with-param name="text" select="$var"/>
	    <xsl:with-param name="render" select="$render"/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$rest"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="open-breakage">
  <xsl:param name="prev-g"/>
<!--
  <xsl:variable name="prev-g" 
	      select="preceding::xtf:g[1][ancestor::xtf:l/@xml:id = current()/ancestor::xtf:l/@xml:id]"/>
 -->
  <xsl:choose>
    <xsl:when test="@breakage='damaged' and not($prev-g[@breakage='damaged'])">
<!--      <sup><xsl:text>&#x23A1;</xsl:text></sup> -->
      <xsl:choose>
	<xsl:when test="ancestor::xtf:gloss or @gloss">
	  <xsl:text>&#xB0;</xsl:text>
	</xsl:when>
	<xsl:otherwise>
         <sup><xsl:text>&#xB0;</xsl:text></sup>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="@breakage='missing' and not($prev-g[@breakage='missing'])">
      <xsl:text>[</xsl:text>
    </xsl:when>
    <xsl:when test="@breakage='maybe-missing' and not($prev-g[@breakage='maybe-missing'])">
      <xsl:text>[(</xsl:text>
    </xsl:when>
    <xsl:when test="@breakage='maybe' and not($prev-g[@breakage='maybe'])">
      <xsl:text>(</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="open-sign">
  <xsl:param name="prev-g"/>
  <xsl:choose>
    <xsl:when test="@sign='ed.supplied' and not($prev-g[@sign='ed.supplied'])">
      <xsl:text>&lt;</xsl:text>
    </xsl:when>
    <xsl:when test="@sign='scribe.implied' and not($prev-g[@sign='scribe.implied'])">
      <xsl:text>&lt;(</xsl:text>
    </xsl:when>
    <xsl:when test="@sign='ed.removed' and not($prev-g[@sign='ed.removed'])">
      <xsl:text>&lt;&lt;</xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="close-sign">
  <xsl:param name="next-g"/>
  <xsl:choose>
    <xsl:when test="@sign='ed.removed' and not($next-g[@sign='ed.removed'])">
      <xsl:text>>></xsl:text>
    </xsl:when>
    <xsl:when test="@sign='scribe.implied' and not($next-g[@sign='scribe.implied'])">
      <xsl:text>)></xsl:text>
    </xsl:when>
    <xsl:when test="@sign='ed.supplied' and not($next-g[@sign='ed.supplied'])">
      <xsl:text>></xsl:text>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="close-breakage">
  <xsl:param name="next-g"/>
  <xsl:choose>
    <xsl:when test="@breakage='maybe' and not($next-g[@breakage='maybe'])">
      <xsl:text>)</xsl:text>
    </xsl:when>
    <xsl:when test="@breakage='maybe-missing' 
		    and not($next-g[@breakage='maybe-missing'])">
      <xsl:text>)]</xsl:text>
    </xsl:when>
    <xsl:when test="@breakage='missing' and not($next-g[@breakage='missing'])">
      <xsl:text>]</xsl:text>
    </xsl:when>
    <xsl:when test="@breakage='damaged' and not($next-g[@breakage='damaged'])">
<!--      <sup><xsl:text>&#x23A4;</xsl:text></sup> -->
      <xsl:choose>
	<xsl:when test="ancestor::xtf:gloss or @gloss">
	  <xsl:text>&#xB0;</xsl:text>
	</xsl:when>
	<xsl:otherwise>
         <sup><xsl:text>&#xB0;</xsl:text></sup>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
