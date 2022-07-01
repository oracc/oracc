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
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:note="http://oracc.org/ns/note/1.0"
    xmlns:xtr="http://oracc.org/ns/xtr/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="draw number dr3d dom xforms xsd xsi form script ooow oooc ooo math svg xh xtr xpd
			     office style text table fo xlink dc meta chart oracc gdl cbd xtf note"
    >

<xsl:include href="xpd.xsl"/>

<xsl:template match="office:body">
  <div>
    <xsl:apply-templates mode="go"/>
  </div>
</xsl:template>

<xsl:template mode="go" match="office:text">
  <xsl:apply-templates mode="go"/>
</xsl:template>

<xsl:template mode="go" match="text:p">
  <p><xsl:apply-templates mode="go"/></p>
</xsl:template>

<xsl:template mode="go" match="text:span">
  <span>
    <xsl:attribute name="class">
      <xsl:value-of select="@text:style-name"/>
    </xsl:attribute>
    <xsl:apply-templates mode="go"/>
  </span>
</xsl:template>

<xsl:template mode="go" match="text()">
  <xsl:value-of select="."/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
