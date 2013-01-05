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

<xsl:key name="have-lem" match="l" use="@n"/>
<xsl:key name="lem" match="c" use="concat(../@n,',',@n)"/>

<xsl:template match="table:table[count(preceding-sibling::table:table)=0]">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="lem"/>
  </xsl:copy>
</xsl:template>

<xsl:template mode="lem" match="table:table-row">
  <xsl:variable name="print-this">
    <xsl:choose>
      <xsl:when test="starts-with(.,'#lem:')">
	<xsl:text>no</xsl:text>
      </xsl:when>
      <xsl:when test="starts-with(preceding-sibling::*[1]/*/*,'#lem:')">
	<xsl:choose>
	  <xsl:when test="count(.//text:p)=0">
	    <xsl:text>no</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>yes</xsl:text>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>yes</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:if test="$print-this = 'yes'">
    <xsl:copy-of select="."/>
    <xsl:variable name="row">
      <xsl:value-of 
	  select="1+count(preceding-sibling::table:table-row)"/>
    </xsl:variable>
    <xsl:if test="key('have-lem',$row)">
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:apply-templates mode="add-lem">
	  <xsl:with-param name="row" select="$row"/>
	</xsl:apply-templates>
      </xsl:copy>
      <table:table-row table:style-name="ro1">
	<table:table-cell table:number-columns-repeated="256"/>
      </table:table-row>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template mode="add-lem" match="table:table-cell">
  <xsl:param name="row"/>
  <xsl:variable name="col">
    <xsl:value-of select="1+count(preceding-sibling::*)
			  -count(preceding-sibling::*[@table:number-columns-repeated])
			  +sum(preceding-sibling::*/@table:number-columns-repeated)"/>
  </xsl:variable>
  <xsl:variable name="prev"
		select="count(preceding-sibling::table:table-cell)"/>
  <xsl:variable name="more"
		select="count(following-sibling::table:table-cell[*])"/>
  <xsl:copy>
    <xsl:copy-of select="@*[not(self::office:value-type)]"/>
    <xsl:attribute name="office:value-type">
      <xsl:text>string</xsl:text>
    </xsl:attribute>
    <xsl:for-each select="text:p">
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:choose>
	  <xsl:when test="$prev = 0">
	    <xsl:text>#lem: </xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:variable name="lem-node" select="key('lem',concat($row,',',$col))"/>
	    <xsl:choose>
	      <xsl:when test="count($lem-node)>0">
		<xsl:for-each select="$lem-node">
		  <xsl:value-of select="."/>
		  <xsl:if test="position() != last() 
				or $more != 0">
		    <xsl:text>; </xsl:text>
		  </xsl:if>
		</xsl:for-each>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:message>row=<xsl:value-of select="$row"/>:col=<xsl:value-of 
		select="$col"/> has no lem</xsl:message>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:copy>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template mode="add-lem" match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="lem"/>
  </xsl:copy>
</xsl:template>

<xsl:template mode="lem" match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="lem"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="lem">
  <xsl:apply-templates select="office:document-content"/>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>