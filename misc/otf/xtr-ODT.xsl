<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
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
		xmlns:xtr="http://oracc.org/ns/xtr/1.0"
		xmlns:xh="http://www.w3.org/1999/xhtml"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		exclude-result-prefixes="xh xtr xpd">

<xsl:param name="mode" select="'www'"/>
<xsl:param name="project" select="''"/>
<xsl:param name="access" select="''"/>
<xsl:param name="line-id" select="''"/>
<xsl:param name="frag-id" select="''"/>

<xsl:include href="xpd.xsl"/>
<xsl:include href="xtr-label.xsl"/>

<xsl:variable name="render-tlat-style">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'render-tlat-style'"/>
    <xsl:with-param name="default" select="'.'"/>
  </xsl:call-template>
</xsl:variable>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template name="array">
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
</xsl:template>

<xsl:template match="xtr:translation">
  <text:p text:style-name="Heading_20_2">
    <xsl:text>Translation</xsl:text>
  </text:p>
  <table:table table:style-name="Table.{generate-id(.)}"
	       table:name="Table.{generate-id(.)}">
    <xsl:call-template name="xtr-emit-table-columns">
      <xsl:with-param name="count" select="1+@xtr:cols"/>
    </xsl:call-template>
    <xsl:apply-templates/>
  </table:table>
</xsl:template>

<xsl:template match="xh:div[@class='note']"/> <!-- FIXME -->

<xsl:template match="xh:p[@class='tr']">
  <table:table-row>
    <xsl:choose>
      <xsl:when test="last()-position()=1">
	<xsl:attribute name="table:table-style">nobreak</xsl:attribute>
      </xsl:when>
      <xsl:when test="following-sibling::*[1]/@state='ruling'">
	<xsl:attribute name="table:table-style">nobreak</xsl:attribute>
      </xsl:when>
    </xsl:choose>
    <xsl:call-template name="translation-cells"/>
  </table:table-row>
</xsl:template>

<xsl:template name="translation-cells">
  <xsl:variable name="top-node" select="ancestor::xtf:translation"/>
  <table:table-cell office:value-type="string" table:style-name="xtf_lnum_cell">
    <text:p text:style-name="xtf_lnum_par">
      <xsl:choose>
	<xsl:when test="$render-tlat-style='rinap'">
	  <xsl:message>render-tlat-style=rinap</xsl:message>
	  <xsl:value-of select="@xtr:lab-start-label"/>
	  <xsl:if test="string-length(@xtr:lab-end-label)>0">
	    <xsl:value-of select="concat('-',@xtr:lab-end-label)"/>
	  </xsl:if>
	  <xsl:text>)&#xa0;</xsl:text>  
	</xsl:when>
	<xsl:otherwise>
	  <xsl:message>render-tlat-style=none</xsl:message>
	  <xsl:choose>
	    <xsl:when test="@xtr:disp">
	      <xsl:value-of select="@xtr:disp"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="@xtr:label"/>
	    </xsl:otherwise>
	  </xsl:choose>
	  <xsl:text>.&#xa0;</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </text:p>
  </table:table-cell>
  <xsl:choose>
    <xsl:when test="xh:innerp/xh:span[@class='cell']">
      <xsl:for-each select="xh:innerp/xh:span[@class='cell']">
	<xsl:variable name="span" select="@xh:span"/>
	<xsl:call-template name="emit-content-cell">
	  <xsl:with-param name="span" select="$span"/>
	</xsl:call-template>
	<xsl:if test="$span > 1">
	  <xsl:call-template name="emit-covered-cells">
	    <xsl:with-param name="count" select="@span"/>
	  </xsl:call-template>
	</xsl:if>
      </xsl:for-each>
      <xsl:variable name="ecols" select="$top-node/@xtr:cols - sum(xh:innerp/xh:span/@xtr:span)"/>
      <xsl:if test="$ecols > 0">
	<xsl:call-template name="emit-empty-cells">
	  <xsl:with-param name="count" select="$ecols+1"/>
	</xsl:call-template>
      </xsl:if>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="span"
		    select="$top-node/@xtr:cols"/>
      <xsl:choose>
	<xsl:when test="$span > 1">
	  <xsl:call-template name="emit-content-cell">
	    <xsl:with-param name="span" select="$span"/>
	  </xsl:call-template>
	  <xsl:call-template name="emit-covered-cells">
	    <xsl:with-param name="count" select="$top-node/@cols"/>
	  </xsl:call-template>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:call-template name="emit-content-cell">
	    <xsl:with-param name="span" select="$span"/>
	  </xsl:call-template>	  
	</xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xh:a[@class='marker']"/>

<xsl:template match="xh:innerp">
  <xsl:apply-templates/>
  <xsl:if test="following-sibling::xh:innerp">
    <br/><span class="indent"/>
  </xsl:if>
</xsl:template>

<xsl:template match="xh:innerp[@class='tr-comment']"/>

<xsl:template match="xh:span[@class='marker']">
  <xsl:message>xtr-ODT.xsl: translation notes are not yet supported; please complain to Steve</xsl:message>
</xsl:template>

<xsl:template match="xh:span[@class='foreign']">
  <text:span text:style-name="i"><xsl:apply-templates/></text:span>
</xsl:template>

<xsl:template match="xh:span[@class='literal']">
  <text:span text:style-name="i"><xsl:apply-templates/></text:span>
</xsl:template>

<xsl:template match="xh:span[@class='r']">
  <text:span text:style-name="r"><xsl:apply-templates/></text:span>
</xsl:template>

<xsl:template match="xh:span[@class='uncertain']">
  <text:span text:style-name="i"><xsl:apply-templates/></text:span>
</xsl:template>

<xsl:template match="xh:span[@class='i']">
  <text:span text:style-name="i"><xsl:apply-templates/></text:span>
</xsl:template>

<xsl:template match="xh:span[@class='w']">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xh:span[@class='gdl']">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xh:p[@class='dollar']">
  <xsl:call-template name="xtr-dollar"/>
</xsl:template>

<xsl:template match="xh:p[@class='note']"/>

<xsl:template match="xh:p">
  <xsl:message>xtr-ODT.xsl: p's class=<xsl:value-of select="@class"/> not translated to ODF</xsl:message>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xh:span">
  <xsl:message>xtr-ODT.xsl: span's class=<xsl:value-of select="@class"/> not translated to ODF</xsl:message>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template mode="notedump" match="xh:p[@class='note']">
</xsl:template>

<xsl:template match="xh:*">
  <xsl:message>xtr-ODT.xsl: XHTML tag <xsl:value-of select="local-name()"/> not translated to ODF</xsl:message>
</xsl:template>

<xsl:template match="xtr:*">
  <xsl:message>xtr-ODT.xsl: XTR tag <xsl:value-of select="local-name()"/> not translated to ODF</xsl:message>
</xsl:template>

<xsl:template name="xtr-dollar">
  <xsl:variable name="xnonl-class">
    <xsl:choose>
      <xsl:when test="preceding-sibling::*[1][self::xh:p][@class='dollar']">
	<xsl:choose>
	  <xsl:when test="following-sibling::*[1][self::xh:p][@class='dollar']">
	    <xsl:text>-medial</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>-final</xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="following-sibling::*[1][self::xh:p][@class='dollar']">
	<xsl:text>-initial</xsl:text>
      </xsl:when>
      <xsl:otherwise/>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="$render-left-align-nonx = 'yes'">
      <table:table-row>
	<table:table-cell table:number-columns-spanned="2">
	  <xsl:call-template name="do-non-c-or-l"/>
	</table:table-cell>
	<table:covered-table-cell/>
	<xsl:call-template name="emit-tlit-empties">
	  <xsl:with-param name="with-label" select="2"/>
	</xsl:call-template>
      </table:table-row>
    </xsl:when>
    <xsl:otherwise>
      <table:table-row>
	<xsl:choose>
	  <xsl:when test="not(preceding-sibling::*[1])">
	    <table:table-cell> <!-- FIXME: cellspan # of rows in Tlit -->
	      <text:p>
		<xsl:choose>
		  <xsl:when test="$render-labels-for-lnums='yes'">
		    <xsl:value-of select="translate(ancestor-or-self::*[@label][1]/@label,
					  ' ','&#xa0;')"/>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:text>&#xa0;</xsl:text>
		  </xsl:otherwise>
		</xsl:choose>
	      </text:p>
	    </table:table-cell>
	  </xsl:when>
	  <xsl:otherwise> <!-- class="nonlnum" -->
	    <table:table-cell><text:p>&#xa0;</text:p></table:table-cell>
	  </xsl:otherwise>
	</xsl:choose>
	<table:table-cell> <!-- colspan="1" --> <!-- class="nonlbody" -->
	  <xsl:call-template name="do-non-c-or-l"/>
	</table:table-cell>
      </table:table-row>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="xtr-emit-table-columns">
  <xsl:param name="count"/>
  <xsl:variable name="id-node" select="ancestor-or-self::xtr:translation"/>
  <table:table-columns>
    <xsl:for-each select="document('')/*/*[@name='array']/*[position() &lt;= $count]">
      <xsl:variable name="col-style">
	<xsl:number format="A"/>
      </xsl:variable>
      <table:table-column 
	  table:style-name="Table.{concat(generate-id($id-node),'.',$col-style)}"/>
    </xsl:for-each>
  </table:table-columns>
</xsl:template>

</xsl:stylesheet>
