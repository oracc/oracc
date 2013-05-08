<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0"
    xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
    xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
    xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
    xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
    xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0"
    xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    xmlns:dc="http://purl.org/dc/elements/1.1/"
    xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0"
    xmlns:number="urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0"
    xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0"
    xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0"
    xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0"
    xmlns:math="http://www.w3.org/1998/Math/MathML"
    xmlns:form="urn:oasis:names:tc:opendocument:xmlns:form:1.0"
    xmlns:script="urn:oasis:names:tc:opendocument:xmlns:script:1.0"
    xmlns:ooo="http://openoffice.org/2004/office"
    xmlns:ooow="http://openoffice.org/2004/writer"
    xmlns:oooc="http://openoffice.org/2004/calc"
    xmlns:dom="http://www.w3.org/2001/xml-events"
    xmlns:xforms="http://www.w3.org/2002/xforms"
    xmlns:xsd="http://www.w3.org/2001/XMLSchema"
    xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
    xmlns:oracc="http://oracc.org/ns/oracc/1.0"
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:cbd="http://oracc.org/ns/cbd/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    xmlns:xtr="http://oracc.org/ns/xtr/1.0"
    exclude-result-prefixes="xpd xtr"
>

<!-- These are formatters which can be overridden -->
<xsl:import href="xtf-ODT-serial.xsl"/>
<xsl:import href="xtr-ODT.xsl"/>
<xsl:import href="gdl-ODT.xsl"/>
<xsl:import href="bib-ODT.xsl"/>
<xsl:import href="cat-ODT-serial.xsl"/>

<!-- These are library routines -->
<xsl:include href="odt-xpd.xsl"/>

<xsl:template name="array">
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
</xsl:template>

<xsl:variable name="project" select="."/>

<xsl:output method="xml" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:template match="xtf:xtf">
  <xsl:apply-templates select="xtf:transliteration|xtf:score|xtf:composite|xtr:translation"/>
</xsl:template>

<xsl:template match="office:document-content/office:automatic-styles">
  <xsl:copy>
    <xsl:copy-of select="*"/>
    <xsl:for-each select="/*/*/office:body/office:text">
      <xsl:for-each 
	  select="xtf:transliteration|xtr:translation
		  |*/xtf:transliteration|*/xtr:translation
		  |*/*/xtf:transliteration|*/*/xtr:translation
		  |oracc:data|*/oracc:data|*/*/oracc:data">
	<xsl:call-template name="table-setup"/>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="dc:*|office:*|style:*|text:*|table:*|xpd:*|draw:*|oracc:pageno|oracc:figno">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

<xsl:template match="xsl:stylesheet"/>

<xsl:template name="table-setup">
  <xsl:message>table-setup self=<xsl:value-of select="local-name()"/></xsl:message>
  <style:style style:name="Table.{generate-id()}" style:family="table"/>
  <xsl:choose>
    <xsl:when test="self::xtr:translation">
      <xsl:message>Setting up table styles for translation</xsl:message>
      <xsl:call-template name="emit-table-column-styles">
	<xsl:with-param name="count" select="1+@xtr:cols"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="emit-table-column-styles">
	<xsl:with-param name="count" select="1+@cols"/>
  </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="emit-table-column-styles">
  <xsl:param name="count"/>
  <xsl:variable name="id-node" select="."/>
  <xsl:for-each select="document('')/*/*[@name='array']/*[position() &lt;= $count]">
    <xsl:variable name="col-style">
      <xsl:number format="A"/>
    </xsl:variable>
    <xsl:variable name="wid">
      <xsl:choose>
	<xsl:when test="position()=1"><xsl:text>1cm</xsl:text></xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="concat(15.5 div ($count - 1),'cm')"/> <!-- FIXME! Dimens from paper size -->
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <style:style style:family="table-column"
	style:name="Table.{concat(generate-id($id-node),'.',$col-style)}">
      <style:table-column-properties style:column-width="{$wid}"/>
    </style:style>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
