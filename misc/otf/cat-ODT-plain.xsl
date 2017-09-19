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
  <name n="copy">Copy</name>
  <name n="exc_no">Excavation Number</name>
  <name n="prov">Provenience</name>
  <name n="period">Period</name>
  <name n="lit">Bibliography</name>
  <name n="edit">Edited by</name>
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
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="oracc:record">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="oracc:field">
  <xsl:variable name="field-abbrev" select="@oracc:name"/>
  <xsl:variable name="field-name" select="document('')/*/*[@name='field-names']/*[@n=$field-abbrev]"/>
  <xsl:choose>
    <xsl:when test="string-length($field-name) = 0">
      <xsl:message>No display name for field <xsl:value-of select="@oracc:name"/></xsl:message>
    </xsl:when>
    <xsl:otherwise>
      <text:p>
	<text:span text:style-name="b"><xsl:value-of select="$field-name"/>:</text:span>
	<xsl:text> </xsl:text>
	<text:span text:style-name="r"><xsl:apply-templates/></text:span>
      </text:p>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="oracc:br">
  <text:line-break/>
</xsl:template>

</xsl:stylesheet>
