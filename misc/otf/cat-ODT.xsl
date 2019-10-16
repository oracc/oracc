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
  <name n="oip_40"   sig="O"><oracc:br/>OIP 40</name>
  <name n="squeeze"  sig="P">Louvre<oracc:br/>Squeeze</name>
  <name n="frgm"     sig="Q"><oracc:br/>Frgm.</name>
  <name n="section"  sig="R"><oracc:br/>Section</name>
  <name n="fuchs"    sig="S">Fuchs,<oracc:br/>Khorsabad line</name>
  <name n="botta"    sig="T">Botta,<oracc:br/>MdN</name>
  <name n="pub"      sig="U">Publication<oracc:br/>reference</name>
  <name n="text_no"  sig="V">Text<oracc:br/>no.</name>
  <name n="winckler" sig="W">Winckler,<oracc:br/>Sar. 2</name>
  <name n="luckenbill" sig="X">Luckenbill,<oracc:br/>ARAB 2</name>
  <name n="el_amin"  sig="Y">El-Amin,<oracc:br/>Sumer</name>
  <name n="waefler"  sig="Z">Wäfler,<oracc:br/>AOAT 26</name>
  <name n="walker"   sig="0">Walker<oracc:br/>in Albenda</name>
  <name n="isak"     sig="1">Fuchs,<oracc:br/>Khorsabad</name>
  <name n="flandin_mdn" sig="2">Flandin in<oracc:br/>Botta, MdN 1–2</name>
  <name n="flandin_albenda" sig="3">Flandin in<oracc:br/>Albenda</name>
  <name n="maniori"  sig="4">Maniori, Campagne<oracc:br/>di Sargon </name>
  <name n="gadd"     sig="5">Gadd<oracc:br/>prism</name>
  <name n="copy_wt"  sig="6">Published Copy<oracc:br/>(Winckler or Tadmor)</name>
  <name n="copy_fuchs" sig="7">Published Copy<oracc:br/>(Fuchs)</name>
  <name n="saas8"    sig="8">Fuchs,<oracc:br/>SAAS §</name>
  <name n="ziegeln1" sig="9">Marzahn<oracc:br/>and Rost no.</name>
  <name n="cast"     sig="a">Louvre<oracc:br/>Cast</name>
  <name n="delongperier" sig="b"><oracc:br/>de Longpérier</name>
  <name n="text74"   sig="c">Text no. 74</name>
  <name n="topic"    sig="d">Topic</name>
  <name n="date"     sig="e">Date</name>
  <name n="column"   sig="f"><oracc:br/>Column</name>
  <name n="khors_date" sig="g">Date in Khorsabad<oracc:br/>Annals (text nos. 1–6)</name>
  <name n="subject"  sig="h"><oracc:br/>Subject</name>
  <name n="subject"  sig="h"><oracc:br/>Subject</name>
  <name n="prov_s"   sig="i"><oracc:br/>Provenance</name>
  <name n="reg_s"    sig="j">Registration<oracc:br/>Number</name>
  <name n="exc_s"    sig="k">Excavation<oracc:br/>Number</name>
  <name n="xexc_s"   sig="m">Excavation/<oracc:br/>Registration No.</name>
  <name n="mus_s"    sig="n">Museum<oracc:br/>Number</name>
  <name n="gives2"   sig="o">Lines<oracc:br/>Preserved</name>
  <name n="source2"  sig="p"><oracc:br/>Source</name>
</xsl:template>

<xsl:template match="oracc:data[@oracc:type='catalog' or @oracc:type='cattable']">
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
