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
<xsl:include href="gdl-OATF.xsl"/>

<!-- XTF -->

<xsl:template match="x:xtf">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="x:protocols">
</xsl:template>

<xsl:template match="x:transliteration">
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
  <xsl:call-template name="errorx">
    <xsl:with-param name="msg" select="concat('unhandled XTF tag `', name(), $q)"/>
  </xsl:call-template>
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

<xsl:template name="errorx">
  <xsl:param name="msg"/>
  <xsl:message>xtf-OATF.xsl:<xsl:value-of 
  select="ancestor-or-self::*[@xml:id][1]/@xml:id"/>: <xsl:value-of select="$msg"/></xsl:message>
</xsl:template>

<xsl:template match="c:xcl|t:translation"/>

</xsl:stylesheet>
