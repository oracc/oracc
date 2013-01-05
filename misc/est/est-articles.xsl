<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:cbd="http://oracc.org/ns/cbd/1.0"
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:nrm="http://oracc.org/ns/nrm/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:xmd="http://oracc.org/ns/xmd/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    xmlns:xtr="http://oracc.org/ns/xtr/1.0"
    xmlns:e="http://exslt.org/common"
    extension-element-prefixes="e"
    >

<xsl:import href="gdl-OATF.xsl"/>

<xsl:output method="text" encoding="utf-8"/>

<xsl:param name="oracc"/>
<xsl:param name="project"/>
<xsl:param name="lang"/>
<xsl:variable name="urn-type">
  <xsl:choose>
    <xsl:when test="starts-with($lang,'qpn')">
      <xsl:text>name</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>word</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:template match="cbd:entry">
  <e:document href="{concat('01bld/est/', $lang, '/', @xml:id, '.est')}">
    <xsl:value-of select="concat('@uri=/',$project,'/',@n,'/html&#xa;')"/>
    <xsl:value-of select="concat('@title=', $project, ' glossary entry for ', 
			  @n, ' (', cbd:overview/@periods, ')&#xa;')"/>
    <xsl:if test="@icount > 0">
      <xsl:value-of select="concat('@weight=',@icount,'&#xa;')"/>
    </xsl:if>
    <xsl:value-of select="concat('proj=',$project,'&#xa;')"/>
    <xsl:value-of select="concat('urntype=',$urn-type,'&#xa;')"/>
    <xsl:text>loctype=glossary_entry&#xa;</xsl:text>
    <xsl:value-of select="concat('lang=',$lang,'&#xa;')"/>
    <xsl:text>&#xa;</xsl:text>
<!--    <xsl:value-of select="concat(cbd:cf,' ', cbd:gw,' ', cbd:pos, '&#xa;')"/> -->
    <xsl:variable name="cbd-node" select="."/>
    <xsl:text>&#x9;</xsl:text>
    <xsl:for-each select="document('')/*/xsl:template[@name='thirty']/a">
      <xsl:value-of select="concat($cbd-node/cbd:cf,' ', $cbd-node/cbd:gw, ' ', $cbd-node/cbd:pos, ' ')"/>
    </xsl:for-each>
    <xsl:text>&#xa;</xsl:text>
    <xsl:text>Forms: </xsl:text>
    <xsl:for-each select="cbd:forms/cbd:form">
      <xsl:variable name="f">
	<xsl:call-template name="un-curly">
	  <xsl:with-param name="form" select="@n"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="$f"/>
      <xsl:text>&#xa;&#x9;</xsl:text>
      <xsl:for-each select="document('')/*/xsl:template[@name='thirty']/a">
	<xsl:value-of select="concat($f, ' ')"/>
      </xsl:for-each>
      <xsl:text>&#xa;</xsl:text>
    </xsl:for-each>
    <xsl:text>Norms: </xsl:text>
    <xsl:for-each select="cbd:norms/cbd:norm">
      <xsl:variable name="f">
	<xsl:call-template name="un-curly">
	  <xsl:with-param name="form" select="cbd:n"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="$f"/>
      <xsl:text>&#xa;&#x9;</xsl:text>
      <xsl:for-each select="document('')/*/xsl:template[@name='thirty']/a">
	<xsl:value-of select="concat($f, ' ')"/>
      </xsl:for-each>
      <xsl:text>&#xa;</xsl:text>
    </xsl:for-each>
    <xsl:for-each select="cbd:senses/cbd:sense">
      <xsl:value-of select="1+count(preceding-sibling::cbd:sense)"/>
      <xsl:text>. </xsl:text>
      <xsl:variable name="f">
	<xsl:value-of select="cbd:mng"/>
      </xsl:variable>
      <xsl:value-of select="$f"/>
      <xsl:text>&#xa;&#x9;</xsl:text>
      <xsl:for-each select="document('')/*/xsl:template[@name='thirty']/a">
	<xsl:value-of select="concat($f, ' ')"/>
      </xsl:for-each>
      <xsl:text>&#xa;</xsl:text>
    </xsl:for-each>
    <xsl:value-of select="concat('&#x9;', $project,' ', $lang, ' ', @n, ' wrd&#xa;')"/>
    <xsl:value-of select="concat('&#x9;', $project,' ', cbd:overview/@periods, '&#xa;')"/>
  </e:document>
</xsl:template>

<xsl:template name="un-curly">
  <xsl:param name="form"/>
  <xsl:value-of select="$form"/>
</xsl:template>

<xsl:template name="xun-curly">
  <xsl:param name="form"/>
  <xsl:variable name="nocurly">
    <xsl:choose>
      <xsl:when test="starts-with($form, '{')">
	<xsl:value-of select="translate(substring($form,2), '{}', '--')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="translate($form, '{}', '--')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="substring($nocurly,string-length($nocurly))='-'">
      <xsl:value-of select="substring($nocurly,1,string-length($nocurly)-1)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$nocurly"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="thirty">
  <a/>
<!-- OK, so thirty is now a lie
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
  <a/>
 -->
</xsl:template>
</xsl:transform>