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

<xsl:output method="text" encoding="utf-8"/>

<xsl:key name="styles" match="style:style" use="@style:name"/>

<xsl:template match="*">
  <xsl:variable name="style-node" select="key('styles',@text:style-name)"/>
  <xsl:variable name="parent-style-node" select="key('styles',$style-node/@style:parent-style-name)"/>
  <xsl:if test="$style-node">

    <xsl:value-of select="@text:style-name"/>
    <xsl:text>&#9;</xsl:text>

    <xsl:if test="$style-node/*/@style:text-position">
      <xsl:choose>
	<xsl:when test="starts-with($style-node/*/@style:text-position,'super')">
	  <xsl:text>super=</xsl:text>
	  <xsl:value-of select="substring-after($style-node/*/@style:text-position, ' ')"/>
	  <xsl:text>&#x9;</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>sub=</xsl:text>
	  <xsl:value-of select="substring-after($style-node/*/@style:text-position, ' ')"/>
	  <xsl:text>&#x9;</xsl:text>
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

    <xsl:variable name="font-variant">
      <xsl:choose>
	<xsl:when test="$style-node/*/@fo:font-variant">
	  <xsl:value-of select="$style-node/*/@fo:font-variant"/>
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

    <xsl:if test="string-length($font-family)>0">
      <xsl:text>font=</xsl:text>
      <xsl:value-of select="$font-family"/>
      <xsl:text>&#9;</xsl:text>
    </xsl:if>

    <xsl:call-template name="print-var">
      <xsl:with-param name="v" select="$font-style"/>
      <xsl:with-param name="n" select="'style'"/>
    </xsl:call-template>

    <xsl:call-template name="print-var">
      <xsl:with-param name="v" select="$font-weight"/>
      <xsl:with-param name="n" select="'weight'"/>
    </xsl:call-template>

    <xsl:call-template name="print-var">
      <xsl:with-param name="v" select="$font-size"/>
      <xsl:with-param name="n" select="'size'"/>
    </xsl:call-template>

    <xsl:call-template name="print-var">
      <xsl:with-param name="v" select="$font-variant"/>
      <xsl:with-param name="n" select="'variant'"/>
    </xsl:call-template>
    
    <xsl:text>&#xa;</xsl:text>
  </xsl:if>
  <xsl:apply-templates/>

</xsl:template>

<xsl:template name="print-var">
  <xsl:param name="v"/>
  <xsl:param name="n"/>
  <xsl:if test="string-length($v)>0">
    <xsl:value-of select="concat($n,'=',$v,'&#x9;')"/>
  </xsl:if>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
