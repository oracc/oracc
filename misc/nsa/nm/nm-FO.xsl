<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:fo="http://www.w3.org/1999/XSL/Format"
	       xmlns:nm="http://psd.museum.upenn.edu/nm"
	       xsl:version="1.0">

<xsl:param name="step-type" select="'oracci'"/>

<xsl:attribute-set name="h1">
  <xsl:attribute name="space-before.minimum">5pt</xsl:attribute>
  <xsl:attribute name="space-before.optimum">10pt</xsl:attribute>
  <xsl:attribute name="space-before.maximum">15pt</xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
  <xsl:attribute name="keep-with-next.within-page">always</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="step-block">
  <xsl:attribute name="space-before.minimum">5pt</xsl:attribute>
  <xsl:attribute name="space-before.optimum">10pt</xsl:attribute>
  <xsl:attribute name="space-before.maximum">20pt</xsl:attribute>
  <xsl:attribute name="keep-together.within-page">always</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="table-borders">
  <xsl:attribute name="border-start-style">solid</xsl:attribute>
  <xsl:attribute name="border-start-width">1pt</xsl:attribute>
  <xsl:attribute name="border-end-style">solid</xsl:attribute>
  <xsl:attribute name="border-end-width">1pt</xsl:attribute>
  <xsl:attribute name="border-before-style">solid</xsl:attribute>
  <xsl:attribute name="border-before-width">1pt</xsl:attribute>
  <xsl:attribute name="border-after-style">solid</xsl:attribute>
  <xsl:attribute name="border-after-width">1pt</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="ruled">
  <xsl:attribute name="border-end-style">solid</xsl:attribute>
  <xsl:attribute name="border-end-width">.5pt</xsl:attribute>
  <xsl:attribute name="border-after-style">solid</xsl:attribute>
  <xsl:attribute name="border-after-width">.5pt</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="cell">
  <xsl:attribute name="border-end-style">solid</xsl:attribute>
  <xsl:attribute name="border-end-width">.5pt</xsl:attribute>
  <xsl:attribute name="border-after-style">solid</xsl:attribute>
  <xsl:attribute name="border-after-width">.5pt</xsl:attribute>
  <xsl:attribute name="padding-start">3pt</xsl:attribute>
</xsl:attribute-set>

<xsl:template match="/">
  <fo:root>
    <fo:layout-master-set>
      <fo:simple-page-master master-name="title-page">
	<fo:region-body 
	   region-name="body"
	   margin-top="4pc"
	   margin-bottom="4pc"
	   margin-left="1in"
	   margin-right="1in"/>
	<fo:region-before region-name="title" extent="2in"/>
      </fo:simple-page-master>
      <fo:simple-page-master master-name="body-page">
	<fo:region-after region-name="folio" extent="24pt"/>
	<fo:region-body 
	   region-name="body"
	   margin-top="4pc"
	   margin-bottom="4pc"
	   margin-left="1in"
	   margin-right="1in"/>
      </fo:simple-page-master>
      <fo:page-sequence-master master-name="signlist">
	<fo:single-page-master-reference master-reference="title-page"/>
	<fo:repeatable-page-master-reference master-reference="body-page"/>
      </fo:page-sequence-master>
    </fo:layout-master-set>
    <fo:page-sequence master-reference="signlist">
      <fo:static-content flow-name="title">
	<fo:block>N and M draft</fo:block>
      </fo:static-content>
      <fo:static-content flow-name="folio">
	<fo:block font-size="8pt" text-align-last="justify">
	  <xsl:text>N and M draft</xsl:text>
	  <fo:leader/>
	  <xsl:text>[</xsl:text><fo:page-number/><xsl:text>]</xsl:text>
	</fo:block>
      </fo:static-content>
      <fo:flow flow-name="body"
	       font-family="Thryomanes" font-weight="normal">
	<xsl:apply-templates/>
      </fo:flow>
    </fo:page-sequence>
  </fo:root>
</xsl:template>

<xsl:template match="nm:master">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="nm:system">
  <fo:block xsl:use-attribute-sets="h1">
    <xsl:if test="position() > 1">
      <xsl:attribute name="break-before">page</xsl:attribute>
    </xsl:if>
    <xsl:value-of select="@title"/>
    <xsl:value-of select="': place='"/>
    <xsl:value-of select="@place"/>
    <xsl:value-of select="'; period='"/>
    <xsl:value-of select="@time"/>
    <xsl:value-of select="'; usedfor='"/>
    <xsl:value-of select="@usedfor"/>
  </fo:block>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="nm:step">
  <fo:block xsl:use-attribute-sets="step-block">
    <fo:table table-layout="fixed"
	      xsl:use-attribute-sets="table-borders"
	      >
      <fo:table-column column-number="1" column-width="72pt"/>
      <fo:table-column column-number="2" column-width="360pt"/>
      <fo:table-body>
	<fo:table-row keep-with-next.within-page="always">
	  <fo:table-cell display-align="center"
			 padding-start="3pt"
			 border-end-style="solid"
			 border-end-width=".5pt">
	    <fo:block>
	      <fo:wrapper font-weight="bold">
		<xsl:choose>
		  <xsl:when test="$step-type='oracci'">
		    <xsl:value-of select="@atf"/>
		  </xsl:when>
		  <xsl:otherwise>
		<xsl:value-of select="@n"/>
		  </xsl:otherwise>
		</xsl:choose>
	      </fo:wrapper>
	      <xsl:if test="@mev-disp">
		<fo:wrapper font-size="80%">
		  <xsl:text> (</xsl:text>
		  <xsl:value-of select="@mev-disp"/>
		  <xsl:text>)</xsl:text>
		</fo:wrapper>
	      </xsl:if>
	    </fo:block>
	  </fo:table-cell>
	  <fo:table-cell
	     padding-start="6pt">
	    <fo:table table-layout="fixed">
	      <fo:table-column column-number="1" column-width="110pt"/>
	      <fo:table-column column-number="2" column-width="160pt"/>
	      <fo:table-column column-number="3" column-width="84pt"/>
	    <fo:table-body>
	      <xsl:apply-templates select="nm:step-instance|nm:inst-system"/>
	    </fo:table-body>
	  </fo:table>
	  </fo:table-cell>
	</fo:table-row>
      </fo:table-body>
    </fo:table>
  </fo:block>
</xsl:template>

<xsl:template match="nm:step-instance">
  <fo:table-row keep-with-next.within-page="always">
    <fo:table-cell>
      <fo:block font-family="CuneiformClassic"
		><xsl:value-of select="@cc"/></fo:block>
    </fo:table-cell>
    <fo:table-cell>
      <fo:block><xsl:value-of select="@atf"/></fo:block>
    </fo:table-cell>
    <fo:table-cell>
      <fo:block>
	<xsl:choose>
	  <xsl:when test="@aev = '#'">
	    <xsl:value-of select="@aeq"/>
	    <xsl:value-of select="' '"/>
	    <xsl:choose>
	      <xsl:when test="$step-type='oracci'">
		<xsl:variable name="aeu" select="@aeu"/>
		<xsl:value-of 
		   select="ancestor::nm:system/nm:step[@n=$aeu]/@atf"/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="@aeu"/>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="@aev"/>
	  </xsl:otherwise>
	</xsl:choose>
      </fo:block>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="nm:inst-system">
  <fo:table-row keep-with-next.within-page="always">
    <fo:table-cell>
      <fo:block><xsl:text>etc. </xsl:text>
	<xsl:value-of select="@n"/></fo:block>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="nm:examples">
  <fo:block xsl:use-attribute-sets="h1">
    Examples
  </fo:block>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="nm:example">
  <fo:block font-family="CuneiformClassic"
	    ><xsl:value-of select="@cc"/></fo:block>
  <fo:block><xsl:value-of select="@atf"/></fo:block>
</xsl:template>

<xsl:template match="nm:page-break">
  <fo:block break-before="page"/>
</xsl:template>

</xsl:transform>
