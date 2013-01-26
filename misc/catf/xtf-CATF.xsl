<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:x="http://oracc.org/ns/xtf/1.0"
  xmlns:c="http://oracc.org/ns/xcl/1.0"
  xmlns:n="http://oracc.org/ns/norm/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:variable name="lower" select="'abcdefgŋhḫijklmnopqrsšṣtṭuvwxyz'"/>
<xsl:variable name="upper" select="'ABCDEFGŊHḪIJKLMNOPQRSŠṢTṬUVWXYZ'"/>

<xsl:variable name="text-lang">
  <xsl:choose>
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
  <xsl:text>~</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:b|g:r|g:v|g:p">
  <xsl:call-template name="g-begin"/>
  <xsl:apply-templates/>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:s">
  <xsl:call-template name="g-begin"/>
  <xsl:choose>
    <xsl:when test="@g:role='logo'">
      <xsl:value-of select="translate(.,$upper,$lower)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
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
  <xsl:text>|</xsl:text>
  <xsl:apply-templates mode="no-breakage"/>
  <xsl:text>|</xsl:text>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:d">
  <xsl:text>{</xsl:text>
  <xsl:call-template name="g-begin"/>
  <xsl:apply-templates/>
  <xsl:call-template name="g-end"/>
  <xsl:text>}</xsl:text>
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
        <xsl:apply-templates select="*[2]"/>
	<xsl:apply-templates select="g:a|g:m"/>
        <xsl:text>)</xsl:text>
      </xsl:for-each>
     </xsl:when>
     <xsl:when test="@sexified">
       <xsl:value-of select="@sexified"/>
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
      <xsl:message>gdl-CATF: <xsl:value-of select="@g:type"/> not handled</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:prox">
  <!-- Proximity notations are dropped in CATF -->
</xsl:template>

<xsl:template match="g:q">
  <xsl:apply-templates select="*[1]"/>
  <xsl:text>(</xsl:text>
  <xsl:apply-templates select="*[2]"/>
  <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="g:gg|n:g">
  <xsl:choose>
    <xsl:when test="@g:type='correction'">
    </xsl:when>
    <xsl:when test="@g:type='alternation'">
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
	<xsl:if test="not(position()=last())">
	  <xsl:text>/</xsl:text>
	</xsl:if>
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
	<xsl:if test="not(position()=last())">
	  <xsl:text>+</xsl:text>
	</xsl:if>
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="@g:type='logo'">
      <!-- drop grouping in ATF -->
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <!-- TODO: implicit-ligature -->
      <xsl:message>gdl-CATF.xsl: can't handle g:gg with type=<xsl:value-of select="@g:type"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<!-- TODO: split-word support -->
<xsl:template match="g:w">
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

<xsl:template match="g:nonw">
  <xsl:choose>
    <xsl:when test="@type='comment' or @type='dollar'">
      <!-- inline comments and dollar-lines are dropped in CATF -->
    </xsl:when>
    <xsl:when test="@type='excised'">
      <xsl:text>&lt;&lt;</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>&gt;&gt;</xsl:text>
    </xsl:when>
    <xsl:when test="@type='punct'">
      <xsl:value-of select="g:p/@g:type"/>
      <xsl:value-of select="@g:delim"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>xtf-CATF.xsl:<xsl:value-of select="ancestor-or-self::*[@xml:id][1]/@xml:id"/>: can't handle g:nonw with type=<xsl:value-of select="@type"/></xsl:message>
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
    <xsl:otherwise>
      <xsl:message>gdl-CATF:<xsl:value-of select="ancestor-or-self::*[@xml:id]/@xml:id"/>: unhandled g:x/@g:type value <xsl:value-of select="@g:type"/></xsl:message>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="g:*">
  <xsl:message>xtf-CATF:<xsl:value-of select="ancestor-or-self::*[@xml:id][1]/@xml:id"/>: unhandled GDL tag <xsl:value-of select="name()"/></xsl:message>
  <xsl:apply-templates/>
</xsl:template>

<!-- TODO:
  g-surro suport
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
  <xsl:if test="@g:role='logo' and not(following-sibling::*[@g:role='logo'])">
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
    <xsl:otherwise>
      <xsl:value-of select="@g:delim"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="g-status-flags">
  <xsl:if test="@g:collated = 'yes' or g:collated='true'">
    <xsl:text>*</xsl:text>
  </xsl:if>
  <xsl:if test="@g:queried = 'yes' or g:queried='true'">
    <xsl:text>?</xsl:text>
  </xsl:if>
  <xsl:if test="@g:remarked = 'yes' or g:remarked='true'">
    <xsl:text>!</xsl:text>
  </xsl:if>
</xsl:template>

<!-- NORM -->
<!--TODO: check how words, segs and flags interact -->
<xsl:template match="n:w">
  <xsl:apply-templates/>
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
  <xsl:value-of select="concat('&amp;',@xml:id,' = ',@n,'&#xa;')"/>
  <xsl:value-of select="concat('#atf: lang ', $text-lang, '&#xa;')"/>
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
  <xsl:text>&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:column">
  <xsl:if test="not(@implicit='1')">
    <xsl:value-of select="concat('@',type)"/>
    <xslif test="@g:type='object'">
      <xsl:value-of select="concat(' ',@n)"/>
    </xslif>
    <xsl:text>&#xa;</xsl:text>
  </xsl:if>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:cmt|x:note"/>

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
    <xsl:when test="@g:type='discourse'">
      <xsl:text>@</xsl:text>
      <xsl:if test="@endflag='1'">
	<xsl:text>end </xsl:text>
      </xsl:if>
      <xsl:value-of select="concat(@subtype, '&#xa;')"/>
    </xsl:when>
    <xsl:when test="@subtype='fragment'">
      <xsl:value-of select="concat('@fragment ', text(), '&#xa;')"/>
    </xsl:when>
    <xsl:when test="@g:type='division'">
      <xsl:value-of select="concat('@div ',@subtype,' ',@n,' ',text(),'&#xa;')"/>
    </xsl:when>
    <xsl:when test="@g:type='locator'">
      <xsl:value-of select="concat('@m=locator ',@subtype,' ',@n,' ',text(),'&#xa;')"/>      
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
    <xsl:when test="starts-with(., '(')">
      <xsl:apply-templates/>
    </xsl:when>
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

<xsl:template match="x:v|x:ag">
  <!-- Variants are dropped in CATF -->
  <!-- Alignment groups are dropped in CATF -->
</xsl:template>

<xsl:template match="x:*">
  <xsl:message>gdl-OATF: xtf:<xsl:value-of select="name()"/> not handled</xsl:message>
  <xsl:apply-templates/>  
</xsl:template>

<xsl:template name="lang-open">
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
  <xsl:text>_</xsl:text>
  <xsl:if test="not(starts-with(@g:logolang,'sux'))">
    <xsl:value-of select="concat('%',@g:logolang,' ')"/>
  </xsl:if>
</xsl:template>

<xsl:template name="logo-close">
  <xsl:text>_</xsl:text>
</xsl:template>

</xsl:stylesheet>
