<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:x="http://oracc.org/ns/xtf/1.0"
  xmlns:t="http://oracc.org/ns/xtr/1.0"
  xmlns:c="http://oracc.org/ns/xcl/1.0"
  xmlns:n="http://oracc.org/ns/norm/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:param name="credit" select="''"/>
<xsl:param name="curated" select="'no'"/>
<xsl:param name="project" select="''"/>

<xsl:variable name="lower" select="'abcdefgŋhḫijklmnopqrsšṣtṭuvwxyz'"/>
<xsl:variable name="upper" select="'ABCDEFGŊHḪIJKLMNOPQRSŠṢTṬUVWXYZ'"/>

<xsl:variable name="q"><xsl:text>'</xsl:text></xsl:variable>

<xsl:key name="alias" match="a" use="@g"/>

<xsl:variable name="text-lang">
  <xsl:choose>
    <xsl:when test="contains(/x:transliteration/@xml:lang,'-')">
      <xsl:value-of select="substring-before(/x:transliteration/@xml:lang,'-')"/>
    </xsl:when>
    <xsl:when test="string-length(/x:transliteration/@xml:lang) > 0">
      <xsl:value-of select="/x:transliteration/@xml:lang"/>
    </xsl:when>
    <xsl:when test="contains(/*/x:transliteration/@xml:lang,'-')">
      <xsl:value-of select="substring-before(/*/x:transliteration/@xml:lang,'-')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="/*/x:transliteration/@xml:lang"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<!-- GDL -->

<xsl:template match="g:a">
<!-- This is not clear: can we simply drop these?
  <xsl:text>~</xsl:text>
  <xsl:apply-templates/>
 -->
</xsl:template>

<xsl:template match="g:v|g:p">
  <xsl:call-template name="g-begin"/>
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="alias">
	<xsl:with-param name="g" select="text()|@g:type"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:b|g:r">
  <xsl:call-template name="g-begin"/>
  <xsl:call-template name="alias">
    <xsl:with-param name="g" select="text()|@g:type"/>
  </xsl:call-template>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:s">
  <xsl:call-template name="g-begin"/>
  <xsl:choose>
    <xsl:when test="@g:role='logo'">
      <xsl:choose>
	<xsl:when test="g:b">
	  <xsl:call-template name="alias">
	    <xsl:with-param name="g" select="translate(g:b,$upper,$lower)"/>
	  </xsl:call-template>
	  <xsl:apply-templates select="*[position()>1]"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:call-template name="alias">
	    <xsl:with-param name="g" select="translate(.,$upper,$lower)"/>
	  </xsl:call-template>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="g:b">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="alias">
	<xsl:with-param name="g" select="text()"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template mode="no-breakage" match="g:b|g:r|g:s|g:v">
  <xsl:apply-templates/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:c">
  <xsl:call-template name="g-begin"/>
  <xsl:value-of select="@form"/>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
<!--
  <xsl:call-template name="g-begin"/>
  <xsl:text>|</xsl:text>
  <xsl:apply-templates mode="no-breakage"/>
  <xsl:text>|</xsl:text>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
-->
</xsl:template>

<xsl:template match="g:d">
  <xsl:call-template name="g-begin"/>
  <xsl:text>{</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:f">
  <!-- Form variants (rarely used sub-sub-glyph-form tags) are dropped in C-ATF -->
</xsl:template>

<xsl:template match="g:g">
  <xsl:text>(</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="g:gloss">
  <xsl:text>{{</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}}</xsl:text>
</xsl:template>

<xsl:template match="g:m">
  <xsl:text>@</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:n">
  <xsl:call-template name="g-begin"/>
  <xsl:call-template name="g-n"/>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template mode="no-breakage" match="g:n">
  <xsl:call-template name="g-n"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template name="g-n">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:for-each select="g:b">
        <xsl:apply-templates select="g:r"/>
        <xsl:text>(</xsl:text>
	<xsl:call-template name="alias">
	  <xsl:with-param name="g" select="*[2]"/>
	</xsl:call-template>
	<xsl:apply-templates select="g:a|g:m"/>
        <xsl:text>)</xsl:text>
      </xsl:for-each>
     </xsl:when>
     <xsl:when test="@sexified">
       <xsl:value-of select="@sexified"/>
     </xsl:when>
     <xsl:when test="starts-with(@form, 'n')">
       <xsl:value-of select="@form"/>
     </xsl:when>
     <xsl:otherwise>
       <xsl:apply-templates select="g:r"/>
       <xsl:text>(</xsl:text>
       <xsl:apply-templates select="*[2]"/>
       <xsl:text>)</xsl:text>
     </xsl:otherwise>
  </xsl:choose>
  <xsl:apply-templates select="g:a|g:m"/>
</xsl:template>

<xsl:template match="g:o">
  <xsl:choose>
    <xsl:when test="@g:type='beside'"><xsl:text>.</xsl:text></xsl:when>
    <xsl:when test="@g:type='joining'"><xsl:text>+</xsl:text></xsl:when>
    <xsl:when test="@g:type='containing'"><xsl:text>×</xsl:text></xsl:when>
    <xsl:when test="@g:type='above'"><xsl:text>&amp;</xsl:text></xsl:when>
    <xsl:when test="@g:type='crossing'"><xsl:text>%</xsl:text></xsl:when>
    <xsl:when test="@g:type='opposing'"><xsl:text>@</xsl:text></xsl:when>
    <xsl:when test="@g:type='repeated'">
      <xsl:value-of select="concat(.,'x')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="error">
	<xsl:with-param name="msg" select="concat(@g:type, ' not handled')"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:prox">
  <!-- Proximity notations are dropped in CATF -->
</xsl:template>

<xsl:template match="g:q">
  <xsl:call-template name="g-begin"/>
  <xsl:call-template name="alias">
    <xsl:with-param name="g" select="@form"/>
  </xsl:call-template>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:gg|n:g">
  <xsl:choose>
    <xsl:when test="@g:type='correction'">
      <xsl:apply-templates select="*[1]"/>
      <xsl:if test="count(*)>1">
	<xsl:text>(</xsl:text>
	<xsl:apply-templates select="*[position()>1]"/>
	<xsl:text>)</xsl:text>
      </xsl:if>
    </xsl:when>
    <xsl:when test="@g:type='alternation'">
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
	<!-- no need to emit '/' here because it comes in from @g:delim -->
<!--
	<xsl:if test="not(position()=last())">
	  <xsl:text>/</xsl:text>
	</xsl:if>
 -->
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="@g:type='group'">
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:when>
    <xsl:when test="@g:type='reordering'">
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
	<!-- no need to emit ':' here because it comes in from @g:delim -->
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="@g:type='ligature'">
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
	<!-- no need to emit '+' here because it comes in from @g:delim -->
<!--
	<xsl:if test="not(position()=last())">
	  <xsl:text>+</xsl:text>
	</xsl:if>
 -->
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="@g:type='logo'">
      <xsl:call-template name="logo-open"/>
      <xsl:apply-templates/>
      <xsl:call-template name="logo-close"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="error">
	<xsl:with-param name="msg" select="concat('g:gg with type=', @g:type, ' not handled')"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<!-- TODO: split-word support -->
<xsl:template match="g:w">
  <xsl:call-template name="w-sub"/>
</xsl:template>

<xsl:template match="g:swc">
  <xsl:text>-</xsl:text>
  <xsl:call-template name="w-sub"/>
</xsl:template>

<xsl:template name="w-sub">
  <xsl:choose>
    <xsl:when test="count(preceding-sibling::*) = 0">
      <xsl:call-template name="lang-open"/>
    </xsl:when>
    <xsl:when test="preceding-sibling::*">
      <xsl:if test="not(@xml:lang = preceding-sibling::*[@xml:lang][1]/@xml:lang)">
	<xsl:call-template name="lang-open"/>
      </xsl:if>
    </xsl:when>
  </xsl:choose>
  <xsl:apply-templates/>
  <xsl:choose>
    <xsl:when test="count(following-sibling::*) = 0">
      <xsl:call-template name="lang-close"/>
    </xsl:when>
    <xsl:when test="not(@xml:lang = following-sibling::*[@xml:lang][1]/@xml:lang)">
      <xsl:call-template name="lang-close"/>
    </xsl:when>
    <xsl:otherwise>
      <!-- do nothing -->
    </xsl:otherwise>
  </xsl:choose>
  <xsl:choose>
    <xsl:when test="@contrefs">
      <xsl:text>;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="g-delim"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:nonw">
  <xsl:call-template name="g-begin"/>
  <xsl:choose>
    <xsl:when test="@type='comment'">
      <!-- inline comments are dropped in CATF -->
    </xsl:when>
    <xsl:when test="@type='notelink'">
      <!-- notelinks are dropped in CATF -->
    </xsl:when>
    <xsl:when test="@type='dollar'">
      <xsl:value-of select="concat('($',text(),'$)')"/>
    </xsl:when>
    <xsl:when test="@type='excised'">
      <!-- the << and >> are provided by @g:o -->
<!--      <xsl:text>&lt;&lt;</xsl:text> -->
      <xsl:apply-templates/>
<!--      <xsl:text>&gt;&gt;</xsl:text> -->
    </xsl:when>
    <xsl:when test="@type='punct'">
      <xsl:for-each select="g:p">
	<xsl:call-template name="g-begin"/>
	<xsl:value-of select="@g:type"/>
	<xsl:call-template name="g-end"/>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="error">
	<xsl:with-param name="msg" select="concat('g:nonw with type=', @type, ' not handled')"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-end"/>
  <xsl:choose>
    <xsl:when test="@g:delim">
      <xsl:value-of select="@g:delim"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text> </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:x">
  <xsl:call-template name="g-begin"/>
  <xsl:choose>
    <xsl:when test="@g:type='ellipsis'">
      <xsl:text>...</xsl:text>
    </xsl:when>
    <xsl:when test="@g:type='newline'">
      <xsl:text>;</xsl:text>
    </xsl:when>
    <xsl:when test="@g:type='empty'"/> <!-- carrier for hyphen in @akk{-ir} -->
    <xsl:when test="@g:type='dollar'">
      <xsl:text>($</xsl:text>
      <xsl:value-of select="text()"/>
      <xsl:text>$)</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="error">
	<xsl:with-param name="msg" select="concat('g:x with @g:type=',@g:type,' not handled')"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-end"/>
  <xsl:choose>
    <xsl:when test="@g:delim">
      <xsl:call-template name="g-delim"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
	<xsl:when test="following-sibling::g:w or following-sibling::g:nonw">
	  <xsl:text> </xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <!-- emit nothing if no following g:w/g:nonw ??? -->
	</xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:*">
  <xsl:call-template name="error">
    <xsl:with-param name="msg" select="concat('unhandled GDL tag `', name(), $q)"/>
  </xsl:call-template>
</xsl:template>

<!-- TODO:
  g-gloss, g-surro suport
 -->
<xsl:template name="g-begin">
  <xsl:value-of select="@g:o"/>
  <xsl:if test="@g:role='logo' and not(preceding-sibling::*[1][@g:role='logo'])">
    <xsl:call-template name="logo-open"/>
  </xsl:if>
</xsl:template>

<!-- Varnum is dropped in CATF -->
<!-- Notemarks are dropped in CATF -->
<xsl:template name="g-end">
  <xsl:call-template name="g-status-flags"/>
  <xsl:choose>
    <xsl:when test="string-length(@g:c)>0">
      <xsl:value-of select="@g:c"/>
    </xsl:when>
    <xsl:when test="@g:break='damaged'">
      <xsl:value-of select="'#'"/>
    </xsl:when>
  </xsl:choose>
  <xsl:if test="@g:role='logo' and not(following-sibling::*[1][@g:role='logo'])">
    <xsl:call-template name="logo-close"/>
  </xsl:if>
</xsl:template>

<xsl:template name="g-delim">
  <xsl:choose>
    <xsl:when test="@g:delim='--'">
      <!-- Double-hyphen delimiters (sometimes used to separate name segments) are rendered as single hyphens in C-ATF. -->
      <xsl:text>-</xsl:text>
    </xsl:when>
    <xsl:when test="@g:delim='.' and @g:role='logo'">
      <xsl:text>-</xsl:text>
    </xsl:when>
    <xsl:when test="@g:delim='.' and ../@g:type='logo'">
      <xsl:text>-</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@g:delim"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="g-status-flags">
  <xsl:if test="@g:collated = '1'">
    <xsl:text>*</xsl:text>
  </xsl:if>
  <xsl:if test="@g:queried = '1'">
    <xsl:text>?</xsl:text>
  </xsl:if>
  <xsl:if test="@g:remarked = '1'">
    <xsl:text>!</xsl:text>
  </xsl:if>
</xsl:template>

<!-- NORM -->
<!--TODO: check how words, segs and flags interact -->
<xsl:template match="n:w">
  <xsl:choose>
    <xsl:when test="count(preceding-sibling::*) = 0">
      <xsl:call-template name="lang-open"/>
    </xsl:when>
    <xsl:when test="preceding-sibling::*">
      <xsl:if test="not(@xml:lang = preceding-sibling::*[@xml:lang][1]/@xml:lang)">
	<xsl:call-template name="lang-open"/>
      </xsl:if>
    </xsl:when>
  </xsl:choose>
  <xsl:apply-templates/>
  <xsl:choose>
    <xsl:when test="count(following-sibling::*) = 0">
      <xsl:call-template name="lang-close"/>
    </xsl:when>
    <xsl:when test="not(@xml:lang = following-sibling::*[@xml:lang][1]/@xml:lang)">
      <xsl:call-template name="lang-close"/>
    </xsl:when>
    <xsl:otherwise>
      <!-- do nothing -->
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="n:s">
  <xsl:call-template name="g-begin"/>
  <xsl:apply-templates/>
  <xsl:call-template name="g-end"/>
</xsl:template>

<!-- n:g merged with template for g:gg -->

<xsl:template match="n:word-group">
  <xsl:for-each select="*">
    <xsl:apply-templates/>
    <xsl:if test="not(position()=last())">
      <xsl:text>/</xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template match="n-grouped-word">
  <xsl:apply-templates/>
</xsl:template>

<!-- XTF -->

<xsl:template match="x:xtf">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:protocols">
</xsl:template>

<xsl:template match="x:transliteration">
  <xsl:message>CATF: processing <xsl:value-of select="@project"/>:<xsl:value-of select="@xml:id"/></xsl:message>
  <xsl:value-of select="concat('&amp;',@xml:id,' = ',@n,'&#xa;')"/>
  <xsl:value-of select="concat('#atf: lang ', $text-lang, '&#xa;')"/>
  <xsl:choose>
    <xsl:when test="string-length($credit) > 0">
      <xsl:value-of select="concat('#',$credit)"/>
    </xsl:when>
    <xsl:when test="$curated='no'">
      <xsl:value-of select="'#From Oracc.'"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="'#Maintained on Oracc.'"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:value-of select="concat(' See http://oracc.org/',@project,'/',@xml:id,'&#xa;')"/>
  <xsl:apply-templates/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="x:composite">
  <!-- Composites are dropped in CATF -->
</xsl:template>

<xsl:template match="x:score">
  <!-- Scores are dropped in CATF -->
</xsl:template>

<xsl:template match="x:sealing">
  <xsl:value-of select="concat('@sealing', ' ', @n, '&#xa;')"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:object">
  <xsl:value-of select="concat('@',@type)"/>
  <xslif test="@type='object'">
    <xsl:value-of select="concat(' ',@n)"/>
  </xslif>
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:surface">
  <xsl:value-of select="concat('@',@type)"/>
  <xsl:choose>
    <xsl:when test="@type='surface'">
      <xsl:value-of select="concat(' ',@n)"/>
    </xsl:when>
    <xsl:when test="@type='fragment' or @type='seal'">
      <xsl:value-of select="concat(' ',@n)"/>
    </xsl:when>
  </xsl:choose>
  <xsl:call-template name="g-status-flags"/>
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:column">
  <xsl:if test="not(@implicit='1')">
    <xsl:value-of select="concat('@column ', @n)"/>
    <xsl:value-of select="translate(@label, 'xvi', '')"/>
    <!--<xsl:call-template name="g-status-flags"/>-->
    <xsl:text>&#xa;</xsl:text>
  </xsl:if>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:cmt|x:note"/>

<xsl:template match="x:h">
  <xsl:value-of select="concat('&#xa;', '@h1 ',text(),'&#xa;&#xa;')"/>
</xsl:template>

<xsl:template match="x:lg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:l">
  <xsl:choose>
    <xsl:when test="bil">
      <xsl:text>==</xsl:text><xsl:value-of select="@xml:lang"/><xsl:text> </xsl:text>
    </xsl:when>
    <xsl:when test="nts">
      <xsl:text>=. </xsl:text>
    </xsl:when>
    <xsl:when test="lgs">
      <xsl:text>=: </xsl:text>
    </xsl:when>
    <xsl:when test="gus">
      <!-- Gloss-underneath lines are dropped in CATF -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="concat(@n, '. ')"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:apply-templates/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="x:m">
  <xsl:choose>
    <xsl:when test="@type='discourse'">
      <xsl:text>@</xsl:text>
      <xsl:if test="@endflag='1'">
	<xsl:text>end </xsl:text>
      </xsl:if>
      <xsl:value-of select="concat(@subtype, '&#xa;')"/>
    </xsl:when>
    <xsl:when test="@subtype='fragment'">
      <xsl:value-of select="concat('@fragment ', text(), '&#xa;')"/>
    </xsl:when>
    <xsl:when test="@type='division'">
      <xsl:value-of select="concat('@div ',@subtype,' ',@n,' ',text(),'&#xa;')"/>
    </xsl:when>
    <xsl:when test="@type='locator'">
      <xsl:value-of select="concat('@m=locator ',@subtype,' ',@n,'&#xa;')"/>      
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template match="x:nonx|nonl">
  <xsl:call-template name="x-non-whatever"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="x:nong">
  <xsl:call-template name="x-non-whatever"/>
</xsl:template>

<xsl:template name="x-non-whatever">
  <xsl:text>$ </xsl:text>
  <xsl:choose>
    <xsl:when test="@strict='1'">
      <xsl:apply-templates/>      
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="x:v|x:ag|x:atf">
  <!-- Variants are dropped in CATF -->
  <!-- Alignment groups are dropped in CATF -->
  <!-- Bad ATF files are dropped in CATF -->
</xsl:template>

<xsl:template match="x:*">
  <xsl:call-template name="error">
    <xsl:with-param name="msg" select="concat('unhandled XTF tag `', name(), $q)"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="lang-open">
<!--  <xsl:message>lang-open <xsl:value-of select="@xml:lang"/></xsl:message> -->
  <xsl:if test="not(starts-with(@xml:lang,ancestor::x:transliteration/@xml:lang))">
    <xsl:text>_</xsl:text>
    <xsl:if test="not(starts-with(@xml:lang,'akk'))">
      <xsl:value-of select="concat('%',@xml:lang,' ')"/>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="lang-close">
  <xsl:if test="not(starts-with(@xml:lang,ancestor::x:transliteration/@xml:lang))">
    <xsl:text>_</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="logo-open">
  <xsl:if test="not(ancestor::g:gg[@g:type='logo']) and not(ancestor::g:d)">
    <xsl:text>@_</xsl:text>
    <xsl:variable name="logolang" select="@g:logolang|*[@g:logolang][1]/@g:logolang"/>
    <xsl:if test="string-length($logolang)>0 and not(starts-with($logolang,'sux'))">
      <xsl:value-of select="concat('%',$logolang,' ')"/>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="logo-close">
  <xsl:if test="not(ancestor::g:gg[@g:type='logo']) and not(ancestor::g:d)">
    <xsl:text>_@</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="alias">
  <xsl:param name="g"/>
  <xsl:variable name="a">
    <xsl:for-each select="document('../data/catf-aliases.xml')">
      <xsl:value-of select="key('alias',$g)/text()"/>
    </xsl:for-each>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="string-length($a) > 0">
<!--      <xsl:message><xsl:value-of select="concat($g, '=&gt;',$a)"/></xsl:message> -->
      <xsl:value-of select="$a"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$g"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="error">
  <xsl:param name="msg"/>
  <xsl:message>xtf-CATF.xsl:<xsl:value-of 
  select="ancestor-or-self::*[@xml:id][1]/@xml:id"/>: <xsl:value-of select="$msg"/></xsl:message>
</xsl:template>

<xsl:template match="c:xcl|t:translation"/>

</xsl:stylesheet>
