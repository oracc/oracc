<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xcl="http://oracc.org/ns/xcl/1.0"
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xl="http://www.w3.org/1999/xlink"
  exclude-result-prefixes="xst">

<xsl:output method="text" encoding="utf-8"
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
   indent="no"/>

<xsl:include href="gdl-OATF.xsl"/>

<xsl:variable name="from-chars" select="' ()'"/>
<xsl:variable name="to-chars"   select="'_&lt;>'"/>

<xsl:template match="/">
  <xsl:apply-templates select="/*/xcl:xcl"/>
</xsl:template>

<xsl:template match="xcl:xcl">
  <xsl:variable name="indent" select="'&#x9;'"/>
  <xsl:variable name="n_j">
    <xsl:call-template name="jsonify">
      <xsl:with-param name="text" select="../@n"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:text>{&#xa;</xsl:text>
  <xsl:value-of select="concat($indent,'&quot;type&quot;: &quot;cdl&quot;,&#xa;')"/>
  <xsl:value-of select="concat($indent,'&quot;project&quot;: &quot;',../@project,'&quot;,&#xa;')"/>
  <xsl:value-of select="concat($indent,'&quot;id&quot;: &quot;',../@xml:id,'&quot;,&#xa;')"/>
  <xsl:value-of select="concat($indent,'&quot;n&quot;: &quot;',$n_j,'&quot;,&#xa;')"/>
  <xsl:text>&#x9;&quot;cdl&quot;: [</xsl:text>
  <xsl:apply-templates>
    <xsl:with-param name="indent" select="concat($indent,'&#x9;')"/>
  </xsl:apply-templates>
  <xsl:text>&#xa;&#x9;]</xsl:text>
  <xsl:text>&#xa;}&#xa;</xsl:text>
</xsl:template>

<xsl:template match="xcl:c">
  <xsl:param name="indent"/>
  <xsl:text>{&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"node": "c",&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"type": "</xsl:text><xsl:value-of select="@type"/><xsl:text>",&#xa;</xsl:text>
  <xsl:if test="string-length(@subtype)>0">
    <xsl:value-of select="$indent"/>
    <xsl:text>"subtype": "</xsl:text><xsl:value-of select="@subtype"/><xsl:text>",&#xa;</xsl:text>
  </xsl:if>
  <xsl:if test="string-length(id(@ref)/@xl:role)>0">
    <xsl:value-of select="$indent"/>
    <xsl:text>"role": "</xsl:text><xsl:value-of select="id(@ref)/@xl:role"/><xsl:text>",&#xa;</xsl:text>    
  </xsl:if>
  <!--
  <xsl:if test="string-length(@tag)>0">
    <xsl:value-of select="$indent"/>
    <xsl:text>"tag": "</xsl:text><xsl:value-of select="@tag"/><xsl:text>",&#xa;</xsl:text>
    </xsl:if>
  -->
  <xsl:if test="string-length(@label)>0">
    <xsl:value-of select="$indent"/>
    <xsl:text>"label": "</xsl:text><xsl:value-of select="@label"/><xsl:text>",&#xa;</xsl:text>
  </xsl:if>
  <xsl:if test="string-length(@xml:id)>0">
    <xsl:value-of select="$indent"/>
    <xsl:text>"id": "</xsl:text><xsl:value-of select="@xml:id"/><xsl:text>",&#xa;</xsl:text>
    </xsl:if>
  <xsl:value-of select="$indent"/>
  <xsl:text>&quot;cdl&quot;: [</xsl:text>
  <xsl:apply-templates>
    <xsl:with-param name="indent" select="concat($indent,'&#9;')"/>
  </xsl:apply-templates>
  <xsl:text>&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>]}</xsl:text>
  <xsl:if test="following-sibling::*">
    <xsl:text> , </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="xcl:d">
  <xsl:param name="indent"/>
  <xsl:text>{&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"node": "d",&#xa;</xsl:text>
  <xsl:variable name="id-node" select="id(@ref)"/>
  <xsl:value-of select="$indent"/>
  <xsl:text>"type": "</xsl:text><xsl:value-of select="@type"/><xsl:text>",&#xa;</xsl:text>

  <xsl:choose>
    <xsl:when test="@type='object'">
      <xsl:value-of select="$indent"/>
      <xsl:text>"subtype": "</xsl:text><xsl:value-of select="id(@ref)/@type"/><xsl:text>",&#xa;</xsl:text>
    </xsl:when>
    <xsl:when test="@type='surface'">
      <xsl:value-of select="$indent"/>
      <xsl:text>"subtype": "</xsl:text><xsl:value-of select="id(@ref)/@type"/><xsl:text>",&#xa;</xsl:text>
    </xsl:when>
    <xsl:when test="string-length(@subtype)>0">
      <xsl:value-of select="$indent"/>
      <xsl:text>"subtype": "</xsl:text><xsl:value-of select="@subtype"/><xsl:text>",&#xa;</xsl:text>
    </xsl:when>
  </xsl:choose>

  <!-- maybe emit a text prop; not appropriate to all d node types-->
  <xsl:variable name="d-text">
    <xsl:choose>
      <xsl:when test="@type = 'nonw'">
	<xsl:apply-templates select="$id-node"/>
      </xsl:when>
      <xsl:when test="@type = 'nonx'">
	<xsl:apply-templates select="$id-node"/>
      </xsl:when>
      <xsl:when test="@type = 'line-start'">
	<xsl:value-of select="$id-node/@n"/>
      </xsl:when>
    </xsl:choose>
  </xsl:variable>
  <xsl:if test="string-length($d-text)>0">
    <xsl:variable name="text_j">
      <xsl:call-template name="jsonify">
	<xsl:with-param name="text" select="$d-text"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="$indent"/>
    <xsl:text>"text": "</xsl:text><xsl:value-of select="$text_j"/><xsl:text>",&#xa;</xsl:text>
  </xsl:if>

  <!-- maybe emit a label prop; not appropriate to all d node types-->
  <xsl:variable name="d-label">
    <xsl:choose>
      <xsl:when test="@type = 'surface'">
	<xsl:value-of select="$id-node/@label"/>
      </xsl:when>
      <xsl:when test="@type = 'line-start'">
	<xsl:value-of select="$id-node/@label"/>
      </xsl:when>
    </xsl:choose>
  </xsl:variable>
  <xsl:if test="string-length($d-label)>0">
    <xsl:variable name="text_j">
      <xsl:call-template name="jsonify">
	<xsl:with-param name="text" select="$d-label"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="$indent"/>
    <xsl:text>"label": "</xsl:text><xsl:value-of select="$text_j"/><xsl:text>",&#xa;</xsl:text>
  </xsl:if>

  <xsl:if test="string-length(@ref)>0">
    <xsl:value-of select="$indent"/>
    <xsl:text>"ref": "</xsl:text><xsl:value-of select="@ref"/><xsl:text>"&#xa;</xsl:text>
  </xsl:if>
  <xsl:value-of select="$indent"/>
  <xsl:text>}</xsl:text>
  <xsl:if test="following-sibling::*">
    <xsl:text> , </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="xcl:l">
  <xsl:param name="indent"/>
  <xsl:text>{&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"node": "l",&#xa;</xsl:text>
  <xsl:choose>
    <xsl:when test="string-length(@cof-head)>0">
      <xsl:value-of select="$indent"/>
      <xsl:text>"cof": "tail",&#xa;</xsl:text>
    </xsl:when>
    <xsl:when test="string-length(@cof-tails)>0">
      <xsl:value-of select="$indent"/>
      <xsl:text>"cof": "head",&#xa;</xsl:text>
    </xsl:when>
  </xsl:choose>
  <xsl:if test="string-length(@cof-head)=0">
    <xsl:variable name="text">
      <xsl:apply-templates select="id(@ref)"/>
    </xsl:variable>
    <xsl:if test="string-length($text)>0">
      <xsl:variable name="text_j">
	<xsl:call-template name="jsonify">
	  <xsl:with-param name="text" select="normalize-space($text)"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="$indent"/>
      <xsl:text>"text": "</xsl:text><xsl:value-of select="$text_j"/><xsl:text>",&#xa;</xsl:text>
    </xsl:if>
  </xsl:if>
  <xsl:if test="@inst">
    <xsl:value-of select="$indent"/>
    <xsl:variable name="inst_j">
      <xsl:call-template name="jsonify">
	<xsl:with-param name="text" select="@inst"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:text>"lem": "</xsl:text><xsl:value-of select="$inst_j"/>
    <xsl:text>",&#xa;</xsl:text>
  </xsl:if>
  <xsl:if test="@sig">
    <xsl:value-of select="$indent"/>
    <xsl:variable name="sig_j">
      <xsl:call-template name="jsonify">
	<xsl:with-param name="text" select="@sig"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:text>"sig": "</xsl:text><xsl:value-of select="$sig_j"/>
    <xsl:text>",&#xa;</xsl:text>
  </xsl:if>
  <xsl:value-of select="$indent"/>
  <xsl:text>"ref": "</xsl:text><xsl:value-of select="@ref"/><xsl:text>",&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"id": "</xsl:text><xsl:value-of select="@xml:id"/><xsl:text>"&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>}</xsl:text>
  <xsl:if test="following-sibling::*">
    <xsl:text> , </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template match="xcl:mds"/>

<xsl:template match="xcl:linkbase">
  <xsl:param name="indent"/>
  <xsl:text>&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>{&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"type": "linkbase",&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"linksets": [</xsl:text>
  <xsl:apply-templates>
    <xsl:with-param name="indent" select="concat($indent, '&#9;')"/>
  </xsl:apply-templates>
  <xsl:text>&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>]}</xsl:text>
</xsl:template>

<xsl:template match="xcl:linkset">
  <xsl:param name="indent"/>
  <xsl:text>&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>{&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"title": "</xsl:text><xsl:value-of select="@xl:title"/><xsl:text>",&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"id": "</xsl:text><xsl:value-of select="@xml:id"/><xsl:text>",&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"role": "</xsl:text><xsl:value-of select="@xl:role"/><xsl:text>",&#xa;</xsl:text>
  <xsl:if test="string-length(@sig)>0">
    <xsl:value-of select="$indent"/>
    <xsl:variable name="sig_j">
      <xsl:call-template name="jsonify">
	<xsl:with-param name="text" select="@sig"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:text>"sig": "</xsl:text><xsl:value-of select="$sig_j"/><xsl:text>",&#xa;</xsl:text>
  </xsl:if>
  <xsl:value-of select="$indent"/>
  <xsl:text>&quot;links&quot;: [</xsl:text>
  <xsl:apply-templates>
    <xsl:with-param name="indent" select="concat($indent,'&#9;')"/>
  </xsl:apply-templates>
  <xsl:text>&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>]}</xsl:text>
  <xsl:if test="following-sibling::*">
    <xsl:text> , </xsl:text>
  </xsl:if> 
</xsl:template>

<xsl:template match="xcl:link">
  <xsl:param name="indent"/>
  <xsl:text>&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>{&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>"title": "</xsl:text><xsl:value-of select="@xl:title"/><xsl:text>",&#xa;</xsl:text>
  <xsl:if test="string-length(@xl:role)>0">
    <xsl:value-of select="$indent"/>
    <xsl:text>"role": "</xsl:text><xsl:value-of select="@xl:role"/><xsl:text>",&#xa;</xsl:text>
  </xsl:if>
  <xsl:value-of select="$indent"/>
  <xsl:text>"ref": "</xsl:text><xsl:value-of select="@xl:href"/><xsl:text>"</xsl:text>
  <xsl:text>&#xa;</xsl:text>
  <xsl:value-of select="$indent"/>
  <xsl:text>}</xsl:text>
  <xsl:if test="following-sibling::*">
    <xsl:text> , </xsl:text>
  </xsl:if> 
</xsl:template>

<xsl:variable name="quote-char"><xsl:text>"</xsl:text></xsl:variable>
<xsl:variable name="escaped-quote"><xsl:text>\"</xsl:text></xsl:variable>

<xsl:variable name="slash-char"><xsl:text>\</xsl:text></xsl:variable>
<xsl:variable name="escaped-slash"><xsl:text>\\</xsl:text></xsl:variable>

<xsl:template name="jsonify">
  <xsl:param name="text"/>
  <xsl:variable name="slash-text">
    <xsl:call-template name="escape-slashes">
      <xsl:with-param name="text" select="translate($text,'&#xa;',' ')"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="quote-text">
    <xsl:call-template name="escape-quotes">
      <xsl:with-param name="text" select="$slash-text"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:value-of select="$quote-text"/>
</xsl:template>

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

<xsl:template name="escape-slashes">
  <xsl:param name="text"/>
  <xsl:choose>
    <xsl:when test="contains($text,$slash-char)">
      <xsl:variable name="pre" select="substring-before($text,$slash-char)"/>
      <xsl:variable name="post">
	<xsl:call-template name="escape-slashes">
	  <xsl:with-param name="text" 
		select="substring-after($text,concat($pre,$slash-char))"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat($pre,$escaped-slash,$post)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="render-word">

</xsl:template>

</xsl:stylesheet>
