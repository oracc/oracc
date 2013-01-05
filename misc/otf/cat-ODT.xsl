<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet version="1.0" 
		xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
		xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
		xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
		xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
		xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
		xmlns:xlink="http://www.w3.org/1999/xlink"
		xmlns:oracc="http://oracc.org/ns/oracc/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:xtf="http://oracc.org/ns/xtf/1.0"
		xmlns:xcl="http://oracc.org/ns/xcl/1.0"
		xmlns:xff="http://oracc.org/ns/xff/1.0"
		xmlns:norm="http://oracc.org/ns/norm/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xl="http://www.w3.org/1999/xlink"
		exclude-result-prefixes="g norm xcl xff xtf xl">

<xsl:param name="project"/>

<xsl:template name="field-names">
  <name n="cpn"      sig="A"><oracc:br/>cpn</name>
  <name n="dimen"    sig="B">Dimensions<oracc:br/>(cm)</name>
  <name n="exc_no"   sig="C">Excavation<oracc:br/>Number</name>
  <name n="exemplar" sig="D"><oracc:br/>Ex.</name>
  <name n="fub27"    sig="E"><oracc:br/>FuB 27</name>
  <name n="gives"    sig="F">Lines<oracc:br/>Preserved</name>
  <name n="mus_no"   sig="G">Museum<oracc:br/>Number</name>
  <name n="object"   sig="H"><oracc:br/>Object</name>
  <name n="pho_no"   sig="I">Photograph<oracc:br/>Number</name>
  <name n="prov"     sig="J"><oracc:br/>Provenance</name>
  <name n="reg"      sig="K">Registration<oracc:br/>Number</name>
  <name n="reg_no"   sig="K">Registration<oracc:br/>Number</name>
  <name n="source"   sig="L"><oracc:br/>Source</name>
  <name n="xexc_no"  sig="M">Excavation/<oracc:br/>Registration No.</name>
  <name n="xmus_no"  sig="N">Museum Number/<oracc:br/>Source</name>
</xsl:template>

<xsl:template match="oracc:data[@oracc:type='catalog']">
  <xsl:variable name="width">
    <xsl:choose>
      <xsl:when test="@width">
	<xsl:value-of select="@width"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>default</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="tabname" select="generate-id()"/>
  <table:table table:name="Table.{$tabname}" table:style-name="Table.{$tabname}">
    <table:table-columns>
      <xsl:for-each select="*[1]/oracc:field">
	<table:table-column table:style-name="cat_col_{@oracc:name}_style_{$width}"/>
      </xsl:for-each>
    </table:table-columns>
    <table:table-rows>
      <xsl:apply-templates mode="hdr" select="*[1]"/>
      <xsl:apply-templates mode="cat"/>
    </table:table-rows>
  </table:table>
</xsl:template>

<xsl:template mode="hdr" match="oracc:record">
  <xsl:variable name="signature">
    <xsl:for-each select="oracc:field">
      <xsl:variable name="field-abbrev" select="@oracc:name"/>
      <xsl:value-of select="document('')/*/*[@name='field-names']
			    /*[@n=$field-abbrev]/@sig"/>
    </xsl:for-each>
  </xsl:variable>
  <table:table-row table:style-name="cat_hdr_style">
    <xsl:attribute name="oracc:signature">
      <xsl:value-of select="$signature"/>
    </xsl:attribute>
    <xsl:apply-templates mode="hdr"/>
  </table:table-row>
</xsl:template>

<xsl:template mode="hdr" match="oracc:field">
  <table:table-cell table:style-name="cat_hdr_{@oracc:name}_style">
    <xsl:variable name="field-abbrev" select="@oracc:name"/>
    <xsl:variable name="field-name" select="document('')/*/*[@name='field-names']/*[@n=$field-abbrev]"/>
    <xsl:choose>
      <xsl:when test="string-length($field-name) = 0">
	<xsl:message>No display name for field <xsl:value-of select="@oracc:name"/></xsl:message>
      </xsl:when>
      <xsl:otherwise>
	<text:p text:style-name="CatHeader"><xsl:apply-templates mode="hdr" select="$field-name"/></text:p>
      </xsl:otherwise>
    </xsl:choose>
  </table:table-cell>
</xsl:template>

<xsl:template mode="hdr" match="oracc:br">
  <text:line-break/>
</xsl:template>

<xsl:template mode="hdr" match="text()">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template mode="cat" match="oracc:record">
  <table:table-row table:style-name="cat_row_style">
    <xsl:apply-templates mode="cat"/>
  </table:table-row>
</xsl:template>

<xsl:template mode="cat" match="oracc:field">
  <xsl:variable name="sname">
    <xsl:choose>
      <xsl:when test="count(ancestor::oracc:record/following-sibling::*) = 0">
	<xsl:value-of select="concat('cat_last_',@oracc:name,'_style')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="concat('cat_cell_',@oracc:name,'_style')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>	
  <table:table-cell table:style-name="{$sname}">
    <text:p text:style-name="CatBody">
      <xsl:apply-templates/>
    </text:p>
  </table:table-cell>
</xsl:template>

</xsl:stylesheet>
