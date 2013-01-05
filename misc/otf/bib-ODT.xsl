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

<xsl:template match="oracc:data[@oracc:type='bibliography']">
  <table:table table:style-name="bib_table_style">
    <table:table-columns>
      <xsl:for-each select="*[1]/oracc:field">
	<table:table-column table:style-name="bib_col_{@oracc:name}_style"/>
      </xsl:for-each>
    </table:table-columns>
    <table:table-rows>
      <xsl:apply-templates mode="bib"/>
    </table:table-rows>
  </table:table>
</xsl:template>

<xsl:template mode="bib" match="oracc:record">
  <table:table-row table:style-name="bib_row_style">
    <xsl:apply-templates mode="bib"/>
  </table:table-row>
</xsl:template>

<xsl:template mode="bib" match="oracc:field">
  <table:table-cell table:style-name="bib_cell_{@oracc:name}_style">
    <text:p text:style-name="bib_par_{@oracc:name}_style">
      <xsl:apply-templates/>
    </text:p>
  </table:table-cell>
</xsl:template>

</xsl:stylesheet>
