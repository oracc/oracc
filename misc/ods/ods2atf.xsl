<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
		xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
		xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
		xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
		xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
		xmlns:xlink="http://www.w3.org/1999/xlink"
		>

<xsl:output method="text"/>

<xsl:variable name="init-chars" select="'&amp;@#$|&lt;>='"/>

<xsl:template name="array">
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
</xsl:template>

<xsl:variable name="trans-row-calc" 
	      select="count(/*/*/*/*/table:table-row
                            [starts-with(.,'@translation')][1]
			    /preceding-sibling::table:table-row)"/>

<xsl:variable name="trans-row">
  <xsl:choose>
    <xsl:when test="$trans-row-calc=0">
      <xsl:value-of select="count(/*/*/*/*/table:table-row)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$trans-row-calc"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:template match="table:table[count(preceding-sibling::table:table)=0]">
  <xsl:apply-templates select="table:table-row" mode="print"/>
</xsl:template>

<xsl:template mode="print" match="table:table-row">
  <xsl:choose>
    <xsl:when test="starts-with(table:table-cell[1],'#lem:')">
      <xsl:call-template name="lem-line"/>
    </xsl:when>
    <xsl:when 
	test="string-length(
                translate(
		  substring(table:table-cell[1],1,1),
		  $init-chars, ''
                )
              )
	      = 0">
      <xsl:call-template name="non-line"/>
    </xsl:when>
    <xsl:when test="contains(table:table-cell[1],'. |')">
      <xsl:call-template name="line-no-cells"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="line-with-cells"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="non-line">
  <xsl:value-of select="table:table-cell[1]"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="line-no-cells">
  <xsl:value-of select="substring-before(table:table-cell[1],'. |')"/>
  <xsl:text>.&#x9;| </xsl:text>
  <xsl:value-of select="table:table-cell[2]"/>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="lem-line">
  <xsl:for-each select="table:table-cell">
    <xsl:choose>
      <xsl:when test="count(*)=0
		      and count(following-sibling::*)=0">
	<!-- ignore trailing padding columns -->
      </xsl:when>
      <xsl:when test="@table:number-columns-repeated">
	<xsl:variable name="cell" select="."/>
	<xsl:variable name="n" 
		      select="1+number(@table:number-columns-repeated)"/>
	<xsl:for-each select="document('')
                              /*/*[@name='array']/*[position()&lt;$n]">
	  <xsl:call-template name="lem-cell">
	    <xsl:with-param name="node" select="$cell"/>
	  </xsl:call-template>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="lem-cell"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="lem-cell">
  <xsl:param name="node" select="."/>
  <xsl:value-of select="$node"/>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template name="line-with-cells">
  <xsl:variable name="amp-str">
    <xsl:choose>
      <xsl:when test="count(preceding-sibling::table:table-row)
		      > $trans-row">
	<xsl:text>@&amp;</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>&amp;</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:value-of select="table:table-cell[1]"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:for-each select="table:table-cell[position()>1]">
    <xsl:choose>
      <xsl:when test="count(*)=0
		      and count(following-sibling::*)=0">
	<!-- ignore trailing padding columns -->
      </xsl:when>
      <xsl:when test="@table:number-columns-repeated">
	<xsl:variable name="cell" select="."/>
	<xsl:variable name="n" 
		      select="1+number(@table:number-columns-repeated)"/>
	<xsl:for-each select="document('')
                              /*/*[@name='array']/*[position()&lt;$n]">
	  <xsl:call-template name="tlit-cell">
	    <xsl:with-param name="node" select="$cell"/>
	    <xsl:with-param name="amp-str" select="$amp-str"/>
	  </xsl:call-template>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="tlit-cell">
	  <xsl:with-param name="amp-str" select="$amp-str"/>
	  </xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template name="tlit-cell">
  <xsl:param name="node" select="."/>
  <xsl:param name="amp-str"/>
  <xsl:choose>
    <xsl:when test="@table:number-columns-spanned
		    and not(@table:number-columns-spanned = 1)">
      <xsl:value-of select="concat(' ', $amp-str,
			    @table:number-columns-spanned,' ')"/>
      <xsl:value-of select="$node"/>	
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="concat(' ', $amp-str, ' ')"/>
      <xsl:value-of select="$node"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>