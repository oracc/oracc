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
    exclude-result-prefixes="draw number dr3d dom xforms xsd xsi form script ooow oooc ooo math svg xh xtr xpd"
    >

<xsl:include href="xpd.xsl"/>

<!-- !SERIAL <xsl:key name="tr-id" match="xh:p" use="@xtr:ref|@xtr:sref"/> -->

<!-- These are the XTR headings that do match up with a heading in the XTF -->
<!-- !SERIAL <xsl:key name="tr-h-to-h" match="xh:h1|xh:h2|xh:h3|xh:h4" use="@xtr:hdr-ref"/> -->

<!-- These are the XTR headings that have to be pulled by an XTF line because
     they have no matching XTF heading -->
<!-- !SERIAL <xsl:key name="tr-h-to-l" match="xh:h1|xh:h2|xh:h3|xh:h4" use="@xtr:ref"/> -->

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
  <xsl:message>xtf-ODT-plain</xsl:message>
  <xsl:apply-templates/>
  <text:p text:style-name="Heading_20_2">Notes</text:p>
  <xsl:apply-templates mode="print" select=".//note:text"/>
</xsl:template>

<xsl:template match="xtf:object">
  <xsl:call-template name="object-par"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:surface">
  <xsl:call-template name="surface-par"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:column">
  <xsl:call-template name="column-par"/>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:h">
  <text:p text:style-name="xtf_tlit_hdr">
    <xsl:apply-templates/>
  </text:p>
</xsl:template>

<xsl:template match="xtf:lg">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xtf:l|xtf:v">
  <xsl:call-template name="transliteration-pars"/>
</xsl:template>

<xsl:template name="object-par">
  <xsl:if test="not(@type = 'tablet')">
    <text:p text:style-name="xtf_tlit_hdr"><xsl:value-of select="@type"/></text:p>
  </xsl:if>
</xsl:template>

<xsl:template name="surface-par">
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
    <text:p text:style-name="xtf_tlit_hdr">
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
  </xsl:if>
</xsl:template>

<xsl:template name="column-par">
  <xsl:if test="$render-labels-for-lnums = 'no'">
    <xsl:if test="not(@implicit='1')">
      <xsl:call-template name="column-row-text"/>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="column-par-text">
  <text:p text:style-name="xtf_tlit_hdr">
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
    <!--<xsl:message>x/column-row-text <xsl:value-of select="@label"/> => <xsl:value-of select="$xlabel"/></xsl:message>-->
    <xsl:value-of select="translate($xlabel,$faux-prime,$real-prime)"/>
  </text:p>
</xsl:template>

<xsl:template name="surface-or-column-tlat-dollar">
  <xsl:choose>
    <xsl:when test="$xlat">
      <xsl:apply-templates select="$xlat"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text/> <!-- not sure what is needed here -->
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="transliteration-pars">
  <xsl:variable name="top-node" 
		select="ancestor::xtf:transliteration|ancestor::xtf:composite"/>
  <text:p text:style-name="xtf_tlit_par">
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
    <xsl:apply-templates/>
  </text:p>
</xsl:template>

<xsl:template match="xtf:f">
  <xsl:text> </xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="note:*|xtf:m"/>

<xsl:template mode="print" match="note:text">
  <xsl:message>xtf-ODT-plain processing note:text <xsl:value-of select="note:auto"/></xsl:message>
  <text:p text:style-name="fnote_par">
    <text:span text:style-name="notelabel">
      <xsl:value-of select="@note:auto"/>
    </text:span>
    <xsl:text> </xsl:text>
    <xsl:apply-templates/>
  </text:p>
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
      <text:p> <!-- may need to style this differently -->
	<xsl:call-template name="do-non-c-or-l"/>
      </text:p>
    </xsl:when>
    <xsl:otherwise>
      <text:p>
	<xsl:call-template name="do-non-c-or-l"/>
      </text:p>
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

<xsl:template match="xtf:cmt"/>

</xsl:stylesheet>
