<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
		xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
		xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
		xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
		xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
		xmlns:xlink="http://www.w3.org/1999/xlink"
		xmlns:str="http://exslt.org/strings"
		extension-element-prefixes="str"
		>
<!-- This script is run on the output that goes into content.xml when
     it has the column widths available to it.  This enables us to fix
     the table width from the column widths. -->

<xsl:key name="columns" match="style:style[@style:family='table-column']" use="@style:name"/>
<xsl:key name="table" match="table:table" use="@table:style-name"/>

<xsl:template match="style:style[@style:family='table']">
  <xsl:variable name="col-widths">
    <xsl:for-each select="key('table',@style:name)">
      <xsl:for-each select="table:table-columns/table:table-column">
	<xsl:for-each select="key('columns',@table:style-name)">
	  <xsl:variable name="w" select="*/@style:column-width"/>
	  <xsl:choose>
	    <xsl:when test="contains($w,'in')">
	      <xsl:value-of select="25.4*number(substring-before($w,'in'))"/>
	    </xsl:when>
	    <xsl:when test="contains($w,'cm')">
	      <xsl:value-of select="10*number(substring-before($w,'cm'))"/>
	    </xsl:when>
	    <xsl:when test="contains($w,'mm')">
	      <xsl:value-of select="number(substring-before($w,'mm'))"/>
	    </xsl:when>
	    <xsl:otherwise/>
	  </xsl:choose>
	  <xsl:text> </xsl:text>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:choose>
      <xsl:when test="style:table-properties">
	<xsl:for-each select="style:table-properties">
	  <xsl:copy>
	    <xsl:attribute name="style:width">
	      <xsl:value-of select="sum(str:split($col-widths))"/>
	      <xsl:text>mm</xsl:text>
	    </xsl:attribute>
	    <xsl:attribute name="table:align">center</xsl:attribute>
	    <xsl:copy-of select="@*[not(self::style:width) and not(self::table:align)]"/>
	  </xsl:copy>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<style:table-properties 
	    style:width="{sum(str:split($col-widths))}mm"
	    table:align="center"
	    />
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates select="*[not(self:table-properties)]"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="*|text()">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>