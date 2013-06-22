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
        xmlns:pzip="urn:cleverage:xmlns:post-processings:zip"
 >

<xsl:output method="xml" encoding="utf-8"/>

<xsl:key name="styles" match="style:style" use="@style:name"/>

<xsl:template match="style:*"/>

<xsl:template match="office:body">
  <content>
    <xsl:apply-templates/>
  </content>
</xsl:template>

<xsl:template match="office:*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="text:note">
  <note>
    <xsl:call-template name="set-attr"/>
    <cite>
      <xsl:value-of select="text:note-citation"/>
    </cite>
    <body>
      <xsl:apply-templates select="text:note-body"/>
    </body>
  </note>
</xsl:template>

<xsl:template match="text:section|text:p|text:h|text:span">
  <xsl:element name="{local-name()}">
    <xsl:call-template name="set-attr"/>
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template match="text:tab">
  <xsl:text>&#x9;</xsl:text>
</xsl:template>

<xsl:template match="text:s">
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template match="text:*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template name="set-attr">
  <xsl:variable name="style-node" select="key('styles',@text:style-name)"/>
  <xsl:variable name="parent-style-node" select="key('styles',$style-node/@style:parent-style-name)"/>
  <xsl:if test="$style-node">
    <xsl:attribute name="name">
      <xsl:value-of select="@text:style-name"/>
    </xsl:attribute>
    <xsl:if test="$style-node/*/@style:text-position">
      <xsl:choose>
	<xsl:when test="starts-with($style-node/*/@style:text-position,'super')">
	  <xsl:attribute name="super">
	    <xsl:value-of select="substring-after($style-node/*/@style:text-position, ' ')"/>
	  </xsl:attribute>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:attribute name="sub">
	    <xsl:value-of select="substring-after($style-node/*/@style:text-position, ' ')"/>
	  </xsl:attribute>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:if>
    <xsl:variable name="font-style">
      <xsl:choose>
	<xsl:when test="$style-node/*/@fo:font-style">
	  <xsl:choose>
	    <xsl:when test="$style-node/*/@fo:font-style = 'italic'">
	      <xsl:text>i</xsl:text>
	    </xsl:when>
	    <xsl:when test="$style-node/*/@fo:font-style = 'normal'">
	      <xsl:text>r</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:when>
	<xsl:otherwise>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="font-weight">
      <xsl:choose>
	<xsl:when test="$style-node/*/@fo:font-weight">
	  <xsl:choose>
	    <xsl:when test="$style-node/*/@fo:font-weight = 'bold'">
	      <xsl:text>b</xsl:text>
	    </xsl:when>
	    <xsl:when test="$style-node/*/@fo:font-style = 'extra-bold'">
	      <xsl:text>b</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:when>
	<xsl:otherwise>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="font-size">
      <xsl:choose>
	<xsl:when test="$style-node/*/@fo:font-size">
	  <xsl:value-of select="$style-node/*/@fo:font-size"/>
	</xsl:when>
      </xsl:choose>
    </xsl:variable>
    <xsl:variable name="font-family">
      <xsl:choose>
	<xsl:when test="$style-node/*/@style:font-name">
	  <xsl:value-of select="$style-node/*/@style:font-name"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:choose>
	    <xsl:when test="$parent-style-node">
	      <xsl:value-of select="$parent-style-node/*/@style:font-name"/>
	    </xsl:when>
	  </xsl:choose>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <xsl:attribute name="style">
      <xsl:call-template name="print-var">
	<xsl:with-param name="v" select="$font-style"/>
      </xsl:call-template>
      <xsl:call-template name="print-var">
	<xsl:with-param name="v" select="$font-weight"/>
      </xsl:call-template>
      <xsl:call-template name="print-var">
	<xsl:with-param name="v" select="$font-size"/>
      </xsl:call-template>
    </xsl:attribute>
    <xsl:if test="string-length($font-family)>0">
      <xsl:attribute name="font">
	<xsl:value-of select="$font-family"/>
      </xsl:attribute>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="print-var">
  <xsl:param name="v"/>
  <xsl:if test="string-length($v)>0">
    <xsl:value-of select="concat(' ',$v,' ')"/>
  </xsl:if>
</xsl:template>

</xsl:stylesheet>
