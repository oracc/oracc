<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:fo="http://www.w3.org/1999/XSL/Format"
	       xmlns:nm="http://psd.museum.upenn.edu/nm"
	       xsl:version="1.0">

<xsl:attribute-set name="h1">
  <xsl:attribute name="space-before.minimum">5pt</xsl:attribute>
  <xsl:attribute name="space-before.optimum">10pt</xsl:attribute>
  <xsl:attribute name="space-before.maximum">15pt</xsl:attribute>
  <xsl:attribute name="font-weight">bold</xsl:attribute>
  <xsl:attribute name="keep-with-next.within-page">always</xsl:attribute>
</xsl:attribute-set>

<xsl:attribute-set name="step-block">
  <xsl:attribute name="space-before.minimum">10pt</xsl:attribute>
  <xsl:attribute name="space-before.optimum">15pt</xsl:attribute>
  <xsl:attribute name="space-before.maximum">30pt</xsl:attribute>
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
<!--  <xsl:attribute name="padding-before">3pt</xsl:attribute> -->
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
	   margin-top="1in"
	   margin-bottom="1in"
	   margin-left="1in"
	   margin-right="1in"/>
	<fo:region-before region-name="title" extent="2in"/>
      </fo:simple-page-master>
      <fo:simple-page-master master-name="body-page">
	<fo:region-after region-name="folio" extent="24pt"/>
	<fo:region-body 
	   region-name="body"
	   margin-top="1in"
	   margin-bottom="1in"
	   margin-left="0.5in"
	   margin-right="0.5in"/>
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

<xsl:template match="nm:system">
  <fo:block xsl:use-attribute-sets="h1">
    <xsl:value-of select="@title"/>
  </fo:block>
  <fo:block xsl:use-attribute-sets="sys-info-block">
    <fo:table table-layout="fixed"
	      xsl:use-attribute-sets="table-borders"
	      >
      <fo:table-column column-number="1" column-width="72pt"
		       xsl:use-attribute-sets="ruled"/>
      <fo:table-column column-number="2" column-width="72pt"
		       xsl:use-attribute-sets="ruled"/>
      <fo:table-column column-number="3" column-width="72pt"
		       xsl:use-attribute-sets="ruled"/>
      <fo:table-column column-number="4" column-width="72pt"
		       xsl:use-attribute-sets="ruled"/>
      <fo:table-column column-number="5" column-width="72pt"
		       xsl:use-attribute-sets="ruled"/>
      <fo:table-body>
	<fo:table-row>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block>System</fo:block></fo:table-cell>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block>Period</fo:block></fo:table-cell>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block>Place</fo:block></fo:table-cell>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block>Base</fo:block></fo:table-cell>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block>Modern</fo:block></fo:table-cell>
	</fo:table-row>
	<fo:table-row>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block><xsl:value-of select="@n"/></fo:block></fo:table-cell>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block><xsl:value-of select="@time"/></fo:block></fo:table-cell>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block><xsl:value-of select="@place"/></fo:block></fo:table-cell>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block><xsl:value-of select="@aev"/></fo:block></fo:table-cell>
	  <fo:table-cell xsl:use-attribute-sets="cell">
	    <fo:block><xsl:value-of select="@mev"/></fo:block></fo:table-cell>
	</fo:table-row>
      </fo:table-body>
    </fo:table>
  </fo:block>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="nm:step">
  <fo:block xsl:use-attribute-sets="step-block">
    <fo:table table-layout="fixed"
	      xsl:use-attribute-sets="table-borders"
	      >
      <fo:table-column column-number="1" column-width="54pt"/>
      <fo:table-column column-number="2" column-width="378pt"/>
      <fo:table-body>
	<fo:table-row>
	  <fo:table-cell display-align="center"
			 padding-start="3pt"
			 border-end-style="solid"
			 border-end-width=".5pt">
	    <fo:block>
	      <fo:wrapper font-weight="bold">
		<xsl:value-of select="@n"/>
	      </fo:wrapper>
	    </fo:block>
	  </fo:table-cell>
	  <fo:table-cell
	     padding-start="6pt">
	    <fo:table table-layout="fixed">
	      <fo:table-column column-number="1" column-width="138pt"/>
	      <fo:table-column column-number="2" column-width="138pt"/>
	      <!--   <fo:table-column column-number="3" column-width="54pt"/>
		     <fo:table-column column-number="4" column-width="54pt"/>
		     -->
	    <fo:table-column column-number="3" column-width="54pt"/>
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
  <fo:table-row>
    <fo:table-cell>
      <fo:block font-family="CuneiformClassic"
		><xsl:value-of select="@cc"/></fo:block>
    </fo:table-cell>
    <fo:table-cell>
      <fo:block><xsl:value-of select="@atf"/></fo:block>
    </fo:table-cell>
<!--
    <fo:table-cell>
      <fo:block><xsl:value-of select="@count"/></fo:block>
    </fo:table-cell>
    <fo:table-cell>
      <fo:block><xsl:value-of select="../@unit"/></fo:block>
    </fo:table-cell>
  -->
    <fo:table-cell>
      <fo:block><xsl:value-of select="@aev"/></fo:block>
    </fo:table-cell>
  </fo:table-row>
</xsl:template>

<xsl:template match="nm:inst-system">
  <fo:table-row>
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

</xsl:transform>
