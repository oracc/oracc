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
    xmlns:note="http://oracc.org/ns/note/1.0"
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:xtr="http://oracc.org/ns/xtr/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    exclude-result-prefixes="draw number dr3d dom xforms xsd xsi form script ooow oooc ooo math svg xh xtr xpd"
    >

<xsl:include href="xpd.xsl"/>
<xsl:include href="xtr-label.xsl"/>

 <!--[ancestor::xtr:translation][starts-with(@xml:lang,$trans-default-lang)] -->
<xsl:key name="tr-id"
	 match="xh:p"
	 use="@xtr:ref|@xtr:sref"/>

<!-- These are the XTR headings that do match up with a heading in the XTF -->
<xsl:key name="tr-h-to-h" match="xh:h1|xh:h2|xh:h3|xh:h4" use="@xtr:hdr-ref"/>

<!-- These are the XTR headings that have to be pulled by an XTF line because
     they have no matching XTF heading -->
<xsl:key name="tr-h-to-l" match="xh:h1|xh:h2|xh:h3|xh:h4" use="@xtr:ref"/>

<xsl:variable name="lc" select="'abcdefghijklmnopqrstuvwxyzšŋ'"/>
<xsl:variable name="uc" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZŠŊ'"/>

<xsl:variable name="faux-prime">'</xsl:variable>
<xsl:variable name="real-prime">′</xsl:variable>

<xsl:variable name="opts" select="/*"/>

<xsl:template name="array">
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
  <a/><a/><a/><a/><a/><a/><a/><a/><a/><a/>
</xsl:template>

<xsl:param name="char-rulings" select="0"/>
<xsl:param name="render-surface-inits" select="'yes'"/>
<xsl:param name="render-labels-for-lnums" select="'no'"/>

<xsl:variable name="trans-default-lang">
  <xsl:choose>
    <xsl:when test="/office:document/office:meta/oracc:trans-default-lang">
      <xsl:value-of select="/office:document/office:meta/oracc:trans-default-lang"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="xpd-option">
	<xsl:with-param name="option" select="'trans-default-lang'"/>
	<xsl:with-param name="default" select="'en'"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:variable>

<xsl:variable name="render-lnum-char">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'render-lnum-char'"/>
    <xsl:with-param name="default" select="'.'"/>
  </xsl:call-template>
</xsl:variable>

<xsl:variable name="render-exemplar-char">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'render-exemplar-char'"/>
    <xsl:with-param name="default" select="':'"/>
  </xsl:call-template>
</xsl:variable>

<xsl:variable name="render-left-align-nonx">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'render-left-align-nonx'"/>
    <xsl:with-param name="default" select="'no'"/>
  </xsl:call-template>
</xsl:variable>

<xsl:variable name="labels-node" select="/*/office:meta/xpd:project/xpd:labels"/>

<xsl:template match="xtr:translation"/>

<xsl:template match="xtf:protocols"/>

<xsl:template name="show-attr">
  <xsl:message>
    <xsl:for-each select="@*">
      <xsl:value-of select="concat(local-name(.),'=',.)"/>
      <xsl:text>:</xsl:text>
    </xsl:for-each>
  </xsl:message>
</xsl:template>

<xsl:template match="xtf:transliteration|xtf:score|xtf:composite">
<!--  <xsl:call-template name="show-attr"/> -->
  <xsl:variable name="actual-cols">
    <xsl:choose>
      <xsl:when test="following-sibling::xtr:translation[1]/@xtr:cols">
	<xsl:value-of select="sum(@cols|following-sibling::xtr:translation/@xtr:cols)"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="number(@cols)"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="n" select="translate(@n,' ','_')"/>
  <table:table table:style-name="Table.{generate-id(..)}"
	       table:name="Table.{generate-id(..)}">
    <xsl:choose>
      <!-- default is to render tlit/xlat in single row, but for facing page
	   layout we have to render them as their own tables and the page maker
	   must split them appropriately -->
      <xsl:when test="$opts/@facingpage = '1'">
	<xsl:apply-templates mode="fp"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="emit-table-columns">
	  <xsl:with-param name="count" select="2+$actual-cols"/>
	</xsl:call-template>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </table:table>
</xsl:template>

<xsl:template match="xtf:object">
<!--<xsl:call-template name="object-row"/>-->
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:surface">
  <xsl:call-template name="surface-row"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:column">
  <xsl:call-template name="column-row"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:h">
  <xsl:variable name="xlat-hdrs" select="key('tr-h-to-h',@xml:id)"/>
  <table:table-row table:style-name="nobreak">
    <table:table-cell table:number-columns-spanned="2" office:value-type="string">
      <!-- span all the cols of the tlit -->
      <!-- span additional rows if xlat-hdrs > 1 -->
      <text:p text:style-name="xtf_tlit_hdr">
	<xsl:apply-templates/>
      </text:p>
    </table:table-cell>
    <!-- emit the necessary tlit covered cell -->
    <table:covered-table-cell/>
    <!-- emit the necessary covered cells -->
    <xsl:choose>
      <xsl:when test="count($xlat-hdrs) = 0">
	<!-- no corresponding header -->
	<xsl:call-template name="emit-xlat-empties"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="emit-xlat-hdr">
	  <xsl:with-param name="hdr" select="$xlat-hdrs[1]"/>
	</xsl:call-template>
      </xsl:otherwise>
    </xsl:choose>
  </table:table-row>
  <xsl:if test="count($xlat-hdrs) > 1">
    <xsl:call-template name="emit-xlat-only-hdrs">
      <xsl:with-param name="hdrs" select="$xlat-hdrs[position()>1]"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template match="xtf:lg">
  <table:table-row-group>
    <xsl:apply-templates/>
  </table:table-row-group>
</xsl:template>

<xsl:template match="xtf:l|xtf:v">
  <xsl:message>trans-default-lang=<xsl:value-of select="$trans-default-lang"/></xsl:message>
  <xsl:variable name="xlat-hdrs" select="key('tr-h-to-l',@xml:id)"/>
  <xsl:variable name="this-tlit" select="."/>
  <xsl:if test="count($xlat-hdrs) > 0">
    <xsl:call-template name="emit-xlat-only-hdrs">
      <xsl:with-param name="hdrs" select="$xlat-hdrs"/>
    </xsl:call-template>
  </xsl:if>
  <xsl:variable name="xlat" select="key('tr-id',@xml:id)[starts-with(ancestor::xtr:translation/@xml:lang,$trans-default-lang)]"/>
  <xsl:message>count xlat=<xsl:value-of select="count($xlat)"/></xsl:message>
  <table:table-row>
    <xsl:choose>
      <xsl:when test="last()-position()=1">
	<xsl:attribute name="table:table-style">nobreak</xsl:attribute>
      </xsl:when>
      <xsl:when test="following-sibling::*[1]/@state='ruling'">
	<xsl:attribute name="table:table-style">nobreak</xsl:attribute>
      </xsl:when>
    </xsl:choose>
    <xsl:call-template name="transliteration-cells"/>
    <xsl:choose>
      <xsl:when test="count($xlat)>0">
	<xsl:for-each select="$xlat[1]">
	  <xsl:call-template name="translation-cells"/>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="emit-xlat-covered"/>
      </xsl:otherwise>
    </xsl:choose>
  </table:table-row>
  <xsl:for-each select="$xlat[position()>1]">
    <table:table-row>
      <xsl:for-each select="$this-tlit">
	<xsl:call-template name="emit-tlit-empties">
	  <xsl:with-param name="with-label-cell" select="1"/>
	</xsl:call-template>
      </xsl:for-each>
      <xsl:call-template name="translation-cells"/>
    </table:table-row>
  </xsl:for-each>
</xsl:template>

<xsl:template mode="fp" match="xtf:object">
  <xsl:call-template name="object-row"/>
  <xsl:apply-templates mode="fp"/>
</xsl:template>

<xsl:template mode="fp" match="xtf:surface">
  <xsl:call-template name="surface-row"/>
  <xsl:apply-templates mode="fp"/>
</xsl:template>

<xsl:template mode="fp" match="xtf:column">
  <xsl:call-template name="column-row"/>
  <xsl:apply-templates mode="fp"/>
</xsl:template>

<!-- in fp mode we need to wrap the transliteration rows that
     correspond to a translation block in a row-group so we can easily
     bite off the right size chunks for page rendering -->
<xsl:template mode="fp" match="xtf:l|xtf:v">
  <table:table-row>
    <xsl:call-template name="transliteration-cells"/>
  </table:table-row>
</xsl:template>

<xsl:template name="object-row">
  <xsl:if test="not(@type = 'tablet')">
    <table:table-row>
      <table:table-cell office:value-type="string">
	<text:p><xsl:value-of select="@type"/></text:p>
      </table:table-cell>
    </table:table-row>
  </xsl:if>
</xsl:template>

<xsl:template name="surface-row">
  <xsl:if test="not(@implicit='1')">
    <xsl:variable name="type" select="@type"/>
    <xsl:variable name="flags">
      <xsl:choose>
	<xsl:when test="@gdl:queried='1' and @gdl:remarked='1'">
	  <xsl:text>?!</xsl:text>
	</xsl:when>
	<xsl:when test="@gdl:queried='1'">
	  <xsl:text>?</xsl:text>
	</xsl:when>
	<xsl:when test="@gdl:remarked='1'">
	  <xsl:text>!</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <table:table-row table:style-name="nobreak">
      <table:table-cell table:number-columns-spanned="2" office:value-type="string">
	<text:p>
	  <xsl:choose>
	    <xsl:when test="$labels-node/*[@name=$type]">
	      <xsl:value-of select="concat($labels-node/*[@name=$type]/@value,$flags)"/>
	    </xsl:when>
	    <xsl:when test="$type='surface'">
	      <xsl:value-of select="@label"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:variable name="C1" select="translate(substring(@type,1,1),$lc,$uc)"/>
	      <xsl:value-of select="concat($C1,substring(@type,2), $flags)"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</text:p>
      </table:table-cell>
      <table:covered-table-cell/>
      <xsl:call-template name="surface-or-column-tlat-dollar"/>
    </table:table-row>
  </xsl:if>
</xsl:template>

<xsl:template name="column-row">
  <xsl:if test="$render-labels-for-lnums = 'no'">
    <xsl:if test="not(@implicit='1')">
      <table:table-row table:style-name="nobreak">
	<table:table-cell table:number-columns-spanned="2" office:value-type="string">
	  <xsl:call-template name="column-row-text"/>
	</table:table-cell>
	<table:covered-table-cell/>
	<xsl:call-template name="surface-or-column-tlat-dollar"/>
      </table:table-row>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="column-row-text">
  <text:p>
    <xsl:choose>
      <xsl:when test="$labels-node/*[@name='column']">
	<xsl:value-of select="$labels-node/*[@name='column']/@value"/>
	<xsl:text> </xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>Column </xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:variable name="xlabel">
      <xsl:choose>
	<xsl:when test="starts-with(@label,'o ')">
	  <xsl:value-of select="substring-after(@label,'o ')"/>
	</xsl:when>
	<xsl:when test="starts-with(@label,'o? ')">
	  <xsl:value-of select="substring-after(@label,'o? ')"/>
	</xsl:when>
	<xsl:when test="starts-with(@label,'r ')">
	  <xsl:value-of select="substring-after(@label,'r ')"/>
	</xsl:when>
	<xsl:when test="starts-with(@label,'r? ')">
	  <xsl:value-of select="substring-after(@label,'r? ')"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="@label"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
<!--    <xsl:message>column-row-text <xsl:value-of select="@label"/> => <xsl:value-of select="$xlabel"/></xsl:message> -->
    <xsl:value-of select="translate($xlabel,$faux-prime,$real-prime)"/>
  </text:p>
</xsl:template>

<xsl:template name="surface-or-column-tlat-dollar">
  <xsl:variable name="xlat" select="key('tr-id',@xml:id)"/>
<!--  <xsl:message>surfac-or-column <xsl:value-of 
    select="@xml:id"/> has <xsl:value-of select="count($xlat)"/> tlats</xsl:message> -->
  <xsl:choose>
    <xsl:when test="$xlat">
      <table:table-cell table:number-columns-spanned="1" office:value-type="string">
	<xsl:apply-templates select="$xlat"/>
      </table:table-cell>
    </xsl:when>
    <xsl:otherwise>
      <table:covered-table-cell/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="transliteration-cells">
  <xsl:variable name="top-node" 
		select="ancestor::xtf:transliteration|ancestor::xtf:composite"/>
  <table:table-cell office:value-type="string" table:style-name="xtf_lnum_cell">
    <text:p text:style-name="xtf_lnum_par">
      <xsl:choose>
	<xsl:when test="@varnum">
	  <xsl:choose>
	    <xsl:when test="contains(@varnum,'::')">
	      <xsl:value-of select="concat(translate(substring-after(@varnum,'::'),
				           $faux-prime,$real-prime),
					   $render-exemplar-char)"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="concat(translate(@varnum,$faux-prime,$real-prime),
			           $render-exemplar-char)"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:choose>
	    <xsl:when test="contains(@n,'::')">
	      <xsl:value-of select="concat(translate(substring-after(@n,'::'),
				           $faux-prime,$real-prime),
				           $render-lnum-char)"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="concat(translate(@n,$faux-prime,$real-prime),
				           $render-lnum-char)"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:otherwise>
      </xsl:choose>
    </text:p>
  </table:table-cell>
  <xsl:choose>
    <xsl:when test="xtf:c">
      <xsl:for-each select="xtf:c">
	<xsl:variable name="span" select="@span"/>
	<xsl:call-template name="emit-content-cell">
	  <xsl:with-param name="span" select="$span"/>
	</xsl:call-template>
	<xsl:if test="$span > 1">
	  <xsl:call-template name="emit-covered-cells">
	    <xsl:with-param name="count" select="@span"/>
	  </xsl:call-template>
	</xsl:if>
      </xsl:for-each>
      <xsl:variable name="ecols" select="$top-node/@cols - sum(xtf:c/@span)"/>
      <xsl:if test="$ecols > 0">
	<xsl:call-template name="emit-empty-cells">
	  <xsl:with-param name="count" select="$ecols+1"/>
	</xsl:call-template>
      </xsl:if>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="span"
		    select="$top-node/@cols"/>
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

<xsl:template name="emit-content-cell">
  <xsl:param name="span"/>
  <table:table-cell office:value-type="string" table:style-name="xtf_tlit_cell">
    <xsl:if test="$span > 1">
      <xsl:attribute name="table:number-columns-spanned">
	<xsl:value-of select="$span"/>
      </xsl:attribute>
    </xsl:if>
    <text:p text:style-name="xtf_tlit_par"><xsl:apply-templates/></text:p>
  </table:table-cell>
</xsl:template>

<xsl:template name="tr-cells-span">
  <xsl:param name="rowspan"/>
  <!-- <xsl:message>translation-cells . = <xsl:value-of select="local-name(.)"/></xsl:message> -->
  <xsl:variable name="cells" select="xh:span[@class='cell']"/>
  <xsl:for-each select="$cells">
    <!-- <xsl:message>tr-cells-span hello</xsl:message> -->
    <xsl:variable name="span" select="@xtr:span"/>
    <table:table-cell office:value-type="string" table:style-name="xtf_tlat_cell">
      <xsl:if test="$rowspan > 1">
	<xsl:attribute name="table:number-rows-spanned">
	  <xsl:value-of select="$rowspan"/>
	</xsl:attribute>
      </xsl:if>
      <xsl:if test="$span > 1">
	<xsl:attribute name="table:number-columns-spanned">
	  <xsl:value-of select="$span"/>
	</xsl:attribute>
      </xsl:if>
      <text:p text:style-name="xtf_tlat_par">
	<xsl:if test="not(preceding-sibling::xh:span)">
	  <text:span text:style-name="xtr_label">
	    <xsl:call-template name="xtr-label"/>
	  </text:span>
	</xsl:if>
	<xsl:apply-templates/>
      </text:p>
    </table:table-cell>
    <xsl:if test="@xtr:span and @xtr:span > 1">
      <xsl:call-template name="emit-covered-cells">
	<xsl:with-param name="count" select="@xtr:span"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:for-each>
  <xsl:variable name="ecols" select="ancestor::xtr:translation/@xtr:cols 
				     - sum($cells/@xtr:span)"/>
  <xsl:if test="$ecols > 0">
    <xsl:call-template name="emit-empty-cells">
      <xsl:with-param name="count" select="$ecols+1"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<!--Note that this architecture breaks multi-spans in translation if there are
    also multi paras in a single labeled unit-->
<xsl:template name="tr-cells-innerp">
  <xsl:param name="rowspan"/>
<!--  <xsl:message>tr-cells-innerp . = <xsl:value-of select="local-name(.)"/></xsl:message> -->
  <xsl:variable name="cells" select="*/xh:span[@class='cell'][1]"/>
  <table:table-cell office:value-type="string" table:style-name="xtf_tlat_cell">
    <xsl:variable name="span" select="xh:span/@xtr:span"/>
    <xsl:if test="$rowspan > 1">
      <xsl:attribute name="table:number-rows-spanned">
	<xsl:value-of select="$rowspan"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:if test="$span > 1">
      <xsl:attribute name="table:number-columns-spanned">
	<xsl:value-of select="$span"/>
      </xsl:attribute>
    </xsl:if>
    <xsl:for-each select="xh:innerp[not(@class='tr-comment')]">
      <!-- <xsl:message>tr-cells-innerp hello</xsl:message> -->
      <text:p text:style-name="xtf_tlat_par">
	<xsl:if test="not(preceding-sibling::*)">
	  <xsl:for-each select="ancestor-or-self::*[@xtr:label][1]">
	    <text:span text:style-name="xtr_label">
	      <xsl:call-template name="xtr-label"/>
	    </text:span>
	  </xsl:for-each>
<!--	  <xsl:value-of select="ancestor-or-self::*[@xtr:label][1]/@xtr:label"/> -->
	</xsl:if>
	<text:s/>
	<xsl:apply-templates/>
      </text:p>
    </xsl:for-each>
  </table:table-cell>
  <xsl:if test="@xtr:span and @xtr:span > 1">
    <xsl:call-template name="emit-covered-cells">
      <xsl:with-param name="count" select="@xtr:span"/>
    </xsl:call-template>
  </xsl:if>
  <xsl:variable name="ecols" select="ancestor::xtr:translation/@xtr:cols 
				     - sum($cells/@xtr:span)"/>
  <xsl:if test="$ecols > 0">
    <xsl:call-template name="emit-empty-cells">
      <xsl:with-param name="count" select="$ecols+1"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="translation-cells">
  <xsl:variable name="rowspan">
    <xsl:choose>
      <xsl:when test="ancestor-or-self::xh:p/following-sibling::xh:p[1]/@xtr:overlap">
	<xsl:value-of select="ancestor-or-self::xh:p/@xtr:rows - 1"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="ancestor-or-self::xh:p/@xtr:rows"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="xh:span">
      <xsl:call-template name="tr-cells-span">
	<xsl:with-param name="rowspan" select="$rowspan"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="text()='(SPACER)'">
      <xsl:call-template name="emit-empty-cells">
	<xsl:with-param name="count" select="2"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="@class='dollar'">
      <table:table-cell>
	<text:p>
	  <xsl:choose>
	    <xsl:when test="text()='ruling'">
	      <xsl:attribute name="text:style-name">Horizontal_20_Line</xsl:attribute>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:apply-templates/>
	    </xsl:otherwise>
	  </xsl:choose>
	</text:p>
      </table:table-cell>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="tr-cells-innerp">
	<xsl:with-param name="rowspan" select="$rowspan"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="emit-covered-cells">
  <xsl:param name="count"/>
  <xsl:for-each select="document('')/*/*[@name='array']/*[position() &lt; $count]">
    <table:covered-table-cell/>
  </xsl:for-each>
</xsl:template>

<xsl:template name="emit-empty-cells">
  <xsl:param name="count"/>
  <xsl:for-each select="document('')/*/*[@name='array']/*[position() &lt; $count]">
    <table:table-cell/>
  </xsl:for-each>
 </xsl:template>

<xsl:template name="emit-table-columns">
  <xsl:param name="count"/>
<!--  <xsl:message>emit-table-columns=<xsl:value-of select="$count"/></xsl:message> -->
  <xsl:variable name="id-node" select="ancestor::xtf:xtf"/>
  <table:table-columns>
    <xsl:for-each select="document('')/*/*[@name='array']/*[position() &lt; $count]">
      <xsl:variable name="col-style">
	<xsl:number format="A"/>
      </xsl:variable>
      <table:table-column 
	  table:style-name="Table.{concat(generate-id($id-node),'.',$col-style)}"/>
    </xsl:for-each>
  </table:table-columns>
</xsl:template>

<xsl:template name="emit-tlit-empties">
  <xsl:param name="with-label-cell" select="0"/>
  <xsl:variable name="top-node" 
		select="ancestor::xtf:transliteration|ancestor::xtf:composite"/>
  <xsl:call-template name="emit-empty-cells">
    <xsl:with-param name="count" 
		    select="$top-node/@cols + $with-label-cell"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="emit-xlat-covered">
  <xsl:param name="adjust" select="0"/>
  <xsl:variable name="top-node" 
		select="ancestor::xtf:transliteration|ancestor::xtf:composite"/>
  <xsl:variable name="tr-cols"
		select="$top-node/following-sibling::xtr:translation/@xtr:cols"/>
  <xsl:if test="$tr-cols > 0">
    <xsl:call-template name="emit-covered-cells">
      <xsl:with-param name="count" select="1 + $tr-cols - $adjust"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template name="emit-xlat-empties">
  <xsl:variable name="top-node" 
		select="ancestor::xtf:transliteration|ancestor::xtf:composite"/>
  <xsl:variable name="tr-cols"
		select="$top-node/following-sibling::xtr:translation/@xtr:cols"/>
  <xsl:if test="$tr-cols > 0">
    <xsl:call-template name="emit-empty-cells">
      <xsl:with-param name="count" select="1+$tr-cols"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<!-- Don't change the context in this routine -->
<xsl:template name="emit-xlat-hdr">
  <xsl:param name="hdr"/>
  <table:table-cell office:value-type="string">
    <!-- FIXME: span all the rows of the xlat -->
    <text:p text:style-name="xtf_tlat_hdr">
      <xsl:apply-templates select="$hdr"/>
    </text:p>
  </table:table-cell>
  <xsl:call-template name="emit-xlat-covered">
    <xsl:with-param name="adjust" select="1"/>
  </xsl:call-template>
</xsl:template>

<xsl:template name="emit-xlat-only-hdrs">
  <xsl:param name="hdrs"/>
  <!-- take care to evaluate subroutines with l-node as context -->
  <xsl:variable name="l-node" select="."/>
  <xsl:for-each select="$hdrs">
    <xsl:variable name="h-node" select="."/>
    <xsl:for-each select="$l-node">
      <table:table-row>
	<xsl:call-template name="emit-tlit-empties">
	  <xsl:with-param name="with-label-cell" select="1"/>
	</xsl:call-template>
	<xsl:call-template name="emit-xlat-hdr">
	  <xsl:with-param name="hdr" select="$h-node"/>
	</xsl:call-template>
      </table:table-row>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xtf:note|xtf:m"/>

<xsl:template mode="print" match="note:text">
<!--<xsl:template match="note:text">-->
  <text:note text:id="{@xml:id}"
	     text:note-class="footnote">
    <xsl:choose>
      <xsl:when test="@note:label">
	<text:note-citation text:label="&#x200D;"> <!-- text:label="{@notelabel}">-->
	  <xsl:text>&#x200D;</xsl:text> <!--<xsl:value-of select="@notelabel"/>-->
	</text:note-citation>
      </xsl:when>
      <xsl:otherwise>
	<text:note-citation>
	  <xsl:value-of select="@note:mark"/>
	</text:note-citation>
      </xsl:otherwise>
    </xsl:choose>
    <text:note-body>
      <text:p text:style-name="Footnote">
	<text:span text:style-name="notelabel">
	  <xsl:value-of select="@note:label"/>
	</text:span>
	<xsl:text> </xsl:text>
	<xsl:apply-templates/>
      </text:p>
    </text:note-body>
  </text:note>
</xsl:template>

<xsl:template match="xh:p">
  <xsl:choose>
    <xsl:when test="xh:innerp">
      <xsl:for-each select="xh:innerp">
	<!-- <xsl:message>innerp</xsl:message> -->
	<text:p>
	  <xsl:apply-templates/>
	</text:p>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <text:p>
	<xsl:apply-templates/>
      </text:p>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xh:span"> <!--[@class='i' or @class='r']"> -->
  <text:span>
    <xsl:choose>
      <xsl:when test="@class='cell'"/>
      <xsl:when test="@class='w'"/>
      <xsl:otherwise>
	<xsl:attribute name="text:style-name">
	  <xsl:value-of select="@class"/>
	</xsl:attribute>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:apply-templates/>
  </text:span>
</xsl:template>

<!--
<xsl:template match="xh:span">
  <text:span text:style-name="{@class}">
    <xsl:apply-templates/>
  </text:span>
</xsl:template>
-->

<xsl:template match="xh:innerp">
  <!-- <xsl:message>innerp</xsl:message> -->
</xsl:template>

<xsl:template match="xtf:nonl|xtf:nonx">
  <xsl:variable name="xnonl-class">
    <xsl:choose>
      <xsl:when test="preceding-sibling::*[1][self::xtf:nonl or self::xtf:nonx]">
	<xsl:choose>
	  <xsl:when test="following-sibling::*[1][self::xtf:nonl or self::xtf:nonx]">
	    <xsl:text>-medial</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>-final</xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:when test="following-sibling::*[1][self::xtf:nonl or self::xtf:nonx]">
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
	<xsl:variable name="xlat" select="key('tr-id',@xml:id)"/>
	<xsl:choose>
	  <xsl:when test="count($xlat)>0">
	    <xsl:for-each select="$xlat[1]">
	      <xsl:call-template name="translation-cells"/>
	    </xsl:for-each>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:call-template name="emit-xlat-covered"/>
	  </xsl:otherwise>
	</xsl:choose>	
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
	<xsl:variable name="xlat" select="key('tr-id',@xml:id)"/>
	<xsl:choose>
	  <xsl:when test="count($xlat)>0">
	    <xsl:for-each select="$xlat[1]">
	      <xsl:call-template name="translation-cells"/>
	    </xsl:for-each>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:call-template name="emit-xlat-covered"/>
	  </xsl:otherwise>
	</xsl:choose>
      </table:table-row>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="do-non-c-or-l">
  <text:p> <!-- style:name="noncl"> -->
    <xsl:choose>
      <xsl:when test="@strict=1">
	<xsl:if test="not(@state='ruling')">
	  <xsl:text> (</xsl:text>
	</xsl:if>
	<xsl:choose>
	  <xsl:when test="@scope='impression' and not(starts-with(.,'('))">
	    <xsl:value-of select="concat('impression of ',.)"/>
	  </xsl:when>
	  <xsl:when test="@state='ruling'">
	    <xsl:choose>
	      <xsl:when test="$char-rulings=1">
		<xsl:value-of select="$ruling-line"/>
		<xsl:if test="@flags">
		  <text:span text:style-name="sup">
		    <xsl:value-of select="@flags"/>
		  </text:span>
		</xsl:if>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:attribute name="text:style-name">Horizontal_20_Line</xsl:attribute>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:when>
	  <xsl:when test="@extent='0' or @extent='n'">
	    <xsl:value-of select="@state"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:if test="@state='traces'">
	      <xsl:text>traces</xsl:text>
	      <xsl:if test="@extent"><xsl:text> of </xsl:text></xsl:if>
	    </xsl:if>
	    <xsl:if test="@extent and not(@scope='impression')">
	      <xsl:value-of select="@extent"/>
	      <xsl:text> </xsl:text>
	      <xsl:if test="@extent = 'rest' or @extent = 'start'">
		<xsl:text>of </xsl:text>
	      </xsl:if>
	    </xsl:if>
	    <xsl:if test="not(@state='seal') and not(@state='ruling') and @extent">
	      <xsl:value-of select="@scope"/>
	      <xsl:if test="@extent='n' or @extent>1"><xsl:text>s</xsl:text></xsl:if>
	      <xsl:if test="not(@state='traces')"><xsl:text> </xsl:text></xsl:if>
	    </xsl:if>
	    <xsl:if test="not(@state='traces')">
	      <xsl:value-of select="@state"/>
	    </xsl:if>
	    <xsl:if test="@ref">
	      <xsl:text> </xsl:text>
	      <xsl:value-of select="@ref"/>
	    </xsl:if>
	  </xsl:otherwise>
	</xsl:choose>
	<xsl:if test="not(@state='ruling')">
	  <xsl:text>)</xsl:text>
	</xsl:if>
      </xsl:when>
      <xsl:when test="@type='image'">
	<xsl:variable name="top-node" 
		      select="ancestor::xtf:transliteration|ancestor::xtf:composite"/>
	<img src="/{$top-node/@project}/images/{@ref}.png"/>
      </xsl:when>
      <xsl:when test="@state='other' and text()='SPACER'"/>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </text:p>
</xsl:template>

</xsl:stylesheet>
