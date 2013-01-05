<?xml version='1.0'?>

<!--

 XSL Stylesheet to produce XHTML version of xtl:l element
 content.

 Steve Tinney 06/21/05, for the CDL.

 v1.1.  Placed in the Public Domain.

-->

<xsl:stylesheet version="1.0" 
  xmlns:xtf="http://emegir.info/xtf"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  exclude-result-prefixes="xtf">

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="/">
  <xcu>
    <xsl:apply-templates select=".//xtf:l"/>
  </xcu>
</xsl:template>

<xsl:template match="xtf:nonl">
  <p class="nonl" id="{@xml:id}">
    <xsl:call-template name="do-non-c-or-l"/>
  </p>
</xsl:template>

<xsl:template name="do-non-c-or-l">
    <xsl:text>(</xsl:text>
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
         <xsl:text>[...]</xsl:text>
      </xsl:when>
      <xsl:when test="@type='traces'">
        <xsl:text> (</xsl:text>
        <xsl:value-of select="@type"/>
        <xsl:text> of </xsl:text>
        <xsl:value-of select="@extent"/>
        <xsl:text> signs)</xsl:text>
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
      <xsl:otherwise>
	<xsl:message>NONG type not handled: <xsl:value-of 
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
  <xsl:variable name="node" select="xtf:mpx[1]|."/>
  <xsl:for-each select="$node">
  <p class="l">
    <xsl:attribute name="xml:id"><xsl:value-of select="@xml:id"/></xsl:attribute>
    <xsl:choose>
      <xsl:when test="string-length($href) > 0">
<!--	<sup class="lnum">-->
<!--	    <a href="javascript:showscore('{/*/@xml:id}','{@n}')"> -->
              <xsl:text>(</xsl:text>
	      <xsl:value-of select="@n"/>
	      <xsl:text>)</xsl:text>
<!--            </a> -->
<!--	</sup>-->
 	<xsl:choose>
	  <xsl:when test="xtf:c">
 	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
 	    <xsl:apply-templates/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="string-length($name) > 0">
<!--	<sup class="lnum">
	<span class="lnum">
 	<a name="{$name}">
	  <xsl:text>(</xsl:text>
          <xsl:value-of select="@n"/>
	  <xsl:text>)</xsl:text>
        </a></span>
	</sup> -->
 	<xsl:choose>
	  <xsl:when test="xtf:c">
 	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
 	    <xsl:apply-templates/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="/xtf:score">
        <sup class="lnum"><xsl:value-of select="@n"/><xsl:text>.</xsl:text></sup>
 	<xsl:choose>
	  <xsl:when test="xtf:c">
 	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
 	    <xsl:apply-templates/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
        <sup class="lnum"><xsl:value-of select="@n"/><xsl:text>.</xsl:text></sup>
 	<xsl:choose>
	  <xsl:when test="xtf:c">
 	    <xsl:apply-templates/>
	  </xsl:when>
	  <xsl:otherwise>
 	    <xsl:apply-templates/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:otherwise>
    </xsl:choose>
  </p>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xtf:mpx"/>

<xsl:template match="xtf:n">
  <span class="n"><xsl:apply-templates/></span>
  <xsl:if test="following-sibling::*">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="xtf:c">
  <xsl:apply-templates/>
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
  <xsl:variable name="lang" select="ancestor-or-self::*[@xml:lang][1]/@xml:lang"/>
  <xsl:variable name="rws" select="ancestor-or-self::*[@rws][1]/@rws"/>
  <span class="w" id="{@xml:id}">
    <xsl:for-each select="*">
      <xsl:apply-templates select="."/>
      <xsl:call-template name="maybe-hyphen"/>
    </xsl:for-each>
  </span>
  <xsl:if test="following-sibling::*">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="maybe-hyphen">
  <xsl:if test="not(position() = last())
		and not(@gloss = 'pre')
	        and not(following::xtf:g[1]/@gloss='post')">
    <xsl:text>-</xsl:text>
  </xsl:if>
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

<xsl:template match="xtf:gg">
  <xsl:for-each select="*">
    <xsl:apply-templates select="."/>
    <xsl:if test="not(position() = last())">
      <xsl:text>.</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xtf:cg.gg">
  <xsl:text>(</xsl:text>
  <xsl:apply-templates/>
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
<!--  <sup class="gloss"> -->
    <xsl:apply-templates/>
<!--  </sup> -->
</xsl:template>

<xsl:template match="xtf:g">
  <xsl:param name="igg-verb"/>
  <xsl:choose>
    <xsl:when test="@gloss">
<!--      <sup> -->
        <xsl:call-template name="format-g"/>
<!--      </sup> -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="format-g">
        <xsl:with-param name="igg-verb" select="$igg-verb"/>
      </xsl:call-template>
      <xsl:if test="(local-name(..)='l') 
	and not(position()=last())">
        <xsl:text> </xsl:text>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="format-g">
  <xsl:param name="igg-verb"/>
  <xsl:variable name="prev-g" 
      select="preceding::xtf:g[1][not(ancestor::xtf:l)
				  or ancestor::xtf:l/@xml:id = current()/ancestor::xtf:l/@xml:id]"/>
  <xsl:variable name="next-g" 
      select="following::xtf:g[1][not(ancestor::xtf:l)
				  or ancestor::xtf:l/@xml:id = current()/ancestor::xtf:l/@xml:id]"/>

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

<xsl:template match="xtf:*">
  <xsl:message>untrapped element <xsl:value-of 
     select="local-name()"/></xsl:message>
</xsl:template>

<xsl:template mode="form" match="text()">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="text()">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template name="init-cap">
  <xsl:param name="str"/>
  <xsl:value-of select="translate(substring($str,1,1),
			          'abcdefghijklmnopqrstuvwxyz',
				  'ABCDEFGHIJKLMNOPQRSTUVWXYZ')"/>
  <xsl:value-of select="substring($str,2,string-length($str))"/>
</xsl:template>

<!--NB: this pair deletes ~ from sign indices-->
<xsl:variable name="subdig"
	select="'&#x2080;&#x2081;&#x2082;&#x2083;&#x2084;&#x2085;&#x2086;&#x2087;&#x2088;&#x2089;&#x208A;~'"/>
<xsl:variable name="regdig" select="'0123456789x'"/>

<xsl:template name="render-grapheme">
  <xsl:param name="g"/>
  <xsl:param name="c" select="''"/>
  <xsl:choose>
    <xsl:when test="@gloss or ancestor::xtf:gloss">
      <sup class="gloss">
	<xsl:call-template name="render-grapheme-sub">
	  <xsl:with-param name="g" select="$g"/>
	  <xsl:with-param name="c" select="$c"/>
	</xsl:call-template>
      </sup>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="render-grapheme-sub">
        <xsl:with-param name="g" select="$g"/>
        <xsl:with-param name="c" select="$c"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="render-grapheme-sub">
  <xsl:param name="g"/>
  <xsl:param name="c" select="''"/>
  <xsl:variable name="lc" select="'abcdefghijklmnopqrstuvwxyzšŋ'"/>
  <xsl:variable name="uc" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZŠŊ'"/>
  <xsl:variable name="html-from" select="'ŋŊ'"/>
  <xsl:variable name="html-to"   select="'ĝĜ'"/>

  <!-- render body -->
  <xsl:variable name="body-with-digits">
    <xsl:choose>
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

  <xsl:variable name="body" select="translate($body-with-digits,$subdig,'')"/>

  <xsl:choose>
    <xsl:when test="$c = 'signref'">
      <span class="sign"><xsl:value-of select="translate(translate($body,$lc,$uc),
				      $html-from,$html-to)"/></span>
    </xsl:when>
    <xsl:when test="$c = 'logogram'">
      <span class="logogram"><xsl:value-of select="translate(translate($body,$lc,$uc),
				      $html-from,$html-to)"/></span>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="translate($body,$html-from,$html-to)"/>
    </xsl:otherwise>
  </xsl:choose>

  <!-- render subscript -->
  <xsl:variable name="sub" select="substring($body-with-digits,
					     string-length($body)+1)"/>
  <xsl:if test="string-length($sub)>0">
    <sub><xsl:value-of select="translate($sub,$subdig,$regdig)"/></sub>
  </xsl:if>

  <!-- render post-body stuff: @g, @g~a and @g~a@g are all possible;
	~a@g is not possible without an @-mod just before it -->

  <xsl:variable name="rest" select="substring-after($g,$body-with-digits)"/>
  <xsl:choose>
    <xsl:when test="contains($rest,'~')">
      <xsl:value-of select="substring-before($rest,'~')"/>
      <xsl:variable name="var" select="substring-after($rest,'~')"/>
      <xsl:choose>
  	<xsl:when test="contains($var,'@')">
  	  <sub><xsl:value-of select="substring-before($var,'@')"/></sub>
  	  <xsl:text>@</xsl:text>
  	  <xsl:value-of select="substring-after($var,'@')"/>
  	</xsl:when>
  	<xsl:otherwise>
  	  <sub><xsl:value-of select="$var"/></sub>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$rest"/>
    </xsl:otherwise>
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

<xsl:template name="open-breakage">
  <xsl:param name="prev-g"/>
<!--
  <xsl:variable name="prev-g" 
	      select="preceding::xtf:g[1][not(ancestor::xtf:l) 
					  or ancestor::xtf:l/@xml:id = current()/ancestor::xtf:l/@xml:id]"/>
 -->
  <xsl:choose>
    <xsl:when test="@breakage='damaged' and not($prev-g[@breakage='damaged'])">
<!--      <sup><xsl:text>&#x23A1;</xsl:text></sup> -->
	<sup><xsl:text>&#x00B0;</xsl:text></sup>
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
	<sup><xsl:text>&#x00B0;</xsl:text></sup>
    </xsl:when>
  </xsl:choose>
 </xsl:template>

</xsl:stylesheet>
