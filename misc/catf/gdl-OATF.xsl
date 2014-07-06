<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:c="http://oracc.org/ns/xcl/1.0"
  xmlns:g="http://oracc.org/ns/gdl/1.0"
  xmlns:n="http://oracc.org/ns/norm/1.0/notused"
  xmlns:x="http://oracc.org/ns/xtf/1.0/notused"
  xmlns:r="http://oracc.org/ns/xtr/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:param name="lemm-mode" select="false()"/>
<xsl:param name="repo-mode" select="false()"/>
<xsl:param name="tatf-mode"/>
<xsl:key name="lemm" match="c:l" use="@ref"/>

<xsl:variable name="lemmo" select="'&#x2E20;'"/> <!-- LEFT VERTICAL BAR WITH QUILL -->
<xsl:variable name="lemmc" select="'&#x2E21;'"/> <!-- RIGHT VERTICAL BAR WITH QUILL -->

<xsl:variable name="xlower" select="'abcdefgŋhḫijklmnopqrsšṣtṭuvwxyz'"/>
<xsl:variable name="xupper" select="'ABCDEFGŊHḪIJKLMNOPQRSŠṢTṬUVWXYZ'"/>

<xsl:variable name="quote">'</xsl:variable>

<xsl:output method="text" indent="no" encoding="utf-8"/>

<xsl:template match="g:a">
  <xsl:text>~</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:b">
  <xsl:choose>
    <xsl:when test="g:o">
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:c">
  <xsl:text>|</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>|</xsl:text>
</xsl:template>

<xsl:template match="g:d">
  <xsl:text>{</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
</xsl:template>

<xsl:template match="g:f">
  <xsl:text>\</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:g">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:m">
  <xsl:text>@</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:n">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:for-each select="g:b">
        <xsl:apply-templates select="g:r"/>
	<xsl:if test="string-length(*[2])>0">
	  <xsl:text>(</xsl:text>
	  <xsl:apply-templates select="*[2]"/>
	  <xsl:apply-templates select="g:a|g:m"/>
	  <xsl:text>)</xsl:text>
	</xsl:if>
      </xsl:for-each>
     </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates select="g:r"/>
	<xsl:if test="string-length(*[2])>0">
	  <xsl:text>(</xsl:text>
	  <xsl:apply-templates select="*[2]"/>
	  <xsl:text>)</xsl:text>
	</xsl:if>
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
    <xsl:when test="@g:type='reordered'"><xsl:text>:</xsl:text></xsl:when>
    <xsl:when test="@g:type='repeated'">
      <xsl:value-of select="concat(.,'x')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="error">
	<xsl:with-param name="msg" select="concat('g:o with @g:type=', @g:type, ' not handled')"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:p">
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

<xsl:template match="g:q">
  <xsl:apply-templates select="*[1]"/>
  <xsl:text>(</xsl:text>
  <xsl:apply-templates select="*[2]"/>
  <xsl:text>)</xsl:text>
</xsl:template>

<xsl:template match="g:r|g:s|g:v">
  <xsl:call-template name="g-begin"/>
  <xsl:choose>
    <xsl:when test="self::g:s">
      <xsl:choose>
	<xsl:when test="ancestor::g:d[@g:role='semantic']">
	  <xsl:value-of select="translate(.,$xupper,$xlower)"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:apply-templates/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-end"/>
  <xsl:call-template name="g-delim"/>
</xsl:template>

<xsl:template match="n:w|g:w">
  <xsl:if test="$tatf-mode">
    <xsl:call-template name="tatf-wid"/>
  </xsl:if>
  <xsl:call-template name="w-sub"/>
  <xsl:if test="$lemm-mode">
      <xsl:for-each select="key('lemm',@xml:id)">
	<xsl:value-of select="$lemmo"/>
	<xsl:value-of select="@inst"/>
	<xsl:value-of select="$lemmc"/>
      </xsl:for-each>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="@contrefs">
      <xsl:text>;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="g-delim"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>&#x9;</xsl:text>
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
    <xsl:when test="@g:type='linebreak'">
      <xsl:text> // </xsl:text>
    </xsl:when>
    <xsl:when test="@g:type='word-absent'">
      <xsl:text>—</xsl:text>
    </xsl:when>
    <xsl:when test="@g:type='word-broken'">
      <xsl:text>±</xsl:text>
    </xsl:when>
    <xsl:when test="@g:type='word-linecont'">
      <xsl:text>→</xsl:text>
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
	<xsl:with-param name="msg" select="concat('g:gg with @g:type=', @g:type, ' not handled')"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="g-delim"/>
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
    <xsl:when test="@type='surro'">
      <xsl:apply-templates/> <!-- this is the head of a surrogate, it has its own <s> wrapper -->
    </xsl:when>
    <xsl:when test="@type='vari'">
      <xsl:text>VARI-FIX-NEEDED</xsl:text><xsl:apply-templates/> <!-- FIX ME -->
    </xsl:when>
    <xsl:when test="g:x[@g:type='word-absent']">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:when test="g:x[@g:type='word-broken']">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:when test="g:x[@g:type='word-linecont']">
      <xsl:apply-templates/>
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

<xsl:template match="g:gloss">
  <xsl:text>{{</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}}</xsl:text>
</xsl:template>

<xsl:template match="g:surro|x:surro">
  <xsl:text>{&lt;</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>>}</xsl:text>
</xsl:template>

<xsl:template match="g:swc">
  <xsl:text>-</xsl:text>
  <xsl:call-template name="w-sub"/>
  <xsl:choose>
    <xsl:when test="@contrefs">
      <xsl:text>;</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="g-delim"/>
    </xsl:otherwise>
  </xsl:choose>
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
</xsl:template>

<xsl:template match="g:*">
  <xsl:call-template name="error">
    <xsl:with-param name="msg" select="concat('unhandled GDL tag `', name(), $quote)"/>
  </xsl:call-template>
</xsl:template>

<xsl:template match="c:xcl"/>

<xsl:template match="x:protocols"/>

<xsl:template match="x:transliteration|x:composite|x:object|x:surface|x:column">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:nonx">
  <xsl:value-of select="concat(@xml:id,'&#x9;$&#x9;')"/>
  <xsl:choose>
    <xsl:when test="starts-with(., '(')">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="x:*">
  <xsl:call-template name="error">
    <xsl:with-param name="msg" select="concat('unhandled XTF tag `', name(), $quote)"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="g-begin">
  <xsl:choose>
    <xsl:when test="string-length(@g:o)>0">
      <xsl:value-of select="@g:o"/>
    </xsl:when>
    <xsl:when test="string-length(@g:ho)>0">
      <xsl:value-of select="'⸢'"/>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="g-end">
  <xsl:choose>
    <xsl:when test="string-length(@g:c)>0">
      <xsl:value-of select="@g:c"/>
    </xsl:when>
    <xsl:when test="string-length(@g:hc)>0">
      <xsl:value-of select="'⸣'"/>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="g-delim">
  <xsl:choose>
    <xsl:when test="@g:delim='--'">
      <xsl:text>-</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@g:delim"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="error">
  <xsl:param name="msg"/>
  <xsl:message>gdl-OATF.xsl:<xsl:value-of 
  select="ancestor-or-self::*[@xml:id][1]/@xml:id"/>: <xsl:value-of select="$msg"/></xsl:message>
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
  <!--
  <xsl:if test="not(ancestor::g:gg[@g:type='logo']) and not(ancestor::g:d)">
    <xsl:text>@_</xsl:text>
    <xsl:variable name="logolang" select="@g:logolang|*[@g:logolang][1]/@g:logolang"/>
    <xsl:if test="string-length($logolang)>0 and not(starts-with($logolang,'sux'))">
      <xsl:value-of select="concat('%',$logolang,' ')"/>
    </xsl:if>
  </xsl:if>
  -->
</xsl:template>

<xsl:template name="logo-close">
  <!--
  <xsl:if test="not(ancestor::g:gg[@g:type='logo']) and not(ancestor::g:d)">
    <xsl:text>_@</xsl:text>
  </xsl:if>
  -->
</xsl:template>

<xsl:template name="wid">
  <xsl:if test="string-length(@xml:id)>0">
    <xsl:value-of select="concat($lemmo,@xml:id,$lemmc)"/>
  </xsl:if>
</xsl:template>

<xsl:template name="tatf-wid">
  <xsl:if test="string-length(@xml:id)>0">
    <xsl:value-of select="concat('@=',@xml:id,'=@')"/>
  </xsl:if>
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

</xsl:stylesheet>
