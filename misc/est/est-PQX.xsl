<xsl:transform
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:gdl="http://oracc.org/ns/gdl/1.0"
    xmlns:nrm="http://oracc.org/ns/nrm/1.0"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns:xmd="http://oracc.org/ns/xmd/1.0"
    xmlns:xpd="http://oracc.org/ns/xpd/1.0"
    xmlns:xtf="http://oracc.org/ns/xtf/1.0"
    xmlns:xtr="http://oracc.org/ns/xtr/1.0">

<xsl:import href="gdl-OATF.xsl"/>

<xsl:output method="text" encoding="utf-8"/>

<xsl:param name="oracc"/>
<xsl:param name="project"/>
<xsl:param name="item"/>

<xsl:variable name="path">
  <xsl:value-of select="concat('file://', $oracc, '/bld/', $project, '/')"/>
  <xsl:value-of select="substring($item,1,4)"/>
  <xsl:value-of select="concat('/',$item)"/>
</xsl:variable>

<xsl:variable name="config-node" 
	      select="document(concat('file://', $oracc, '/xml/', $project, '/config.xml'))/*"/>

<xsl:variable name="translation"
	      select="document(concat('file://', $oracc, '/bld/', 
		      $project, '/',
		      substring($item,1,4), '/',
		      $item, '/',
		      $item,
		      '_project-en.xtr'))"/>

<xsl:key name="xtr" match="xh:p" use="@xtr:ref|@xtr:sref"/>

<xsl:template match="/">
<!--  <xsl:message><xsl:value-of select="$path"/></xsl:message> -->
  <xsl:for-each select="document(concat($path,'/',$item,'.xmd'))">
    <xsl:value-of select="concat('@uri=/',$project,'/',$item,'/html&#xa;')"/>
    <xsl:value-of select="concat('@title=', $config-node/xpd:abbrev,' ',$item,'=',*/*/xmd:designation)"/>
    <xsl:choose>
      <xsl:when test="string-length(*/*/xmd:subgenre)>0">
	<xsl:value-of select="concat(' (',*/*/xmd:subgenre,')')"/>
      </xsl:when>
      <xsl:when test="string-length(*/*/xmd:genre)>0">
	<xsl:value-of select="concat(' (',*/*/xmd:genre,')')"/>
      </xsl:when>
    </xsl:choose>
    <xsl:text>&#xa;</xsl:text>

    <xsl:value-of select="concat('proj=',$project,'&#xa;')"/>
    <xsl:text>urntype=text&#xa;</xsl:text>
    <xsl:text>loctype=text&#xa;</xsl:text>
<!--    <xsl:value-of select="concat('lang=',$lang,'&#xa;')"/> -->
    <xsl:text>&#xa;</xsl:text>

    <xsl:for-each select="document(concat($path,'/',$item,'.xtf'))">
      <xsl:apply-templates select="//xtf:l"/>
    </xsl:for-each>
    <xsl:value-of select="concat('&#x9; ', $project, ' ', */*/xmd:id_text|*/*/xmd:id_composite, ' txt&#xa;')"/>
<!--    <xsl:value-of select="concat('&#x9; ', $project, ' ', */*/xmd:id_text|*/*/xmd:id_composite, ' text&#xa;')"/> -->
    <xsl:value-of select="concat('::&#x9; ', */*/xmd:designation, ' txt&#xa;')"/>
    <xsl:if test="string-length(*/*/xmd:museum_no) > 0">
      <xsl:value-of select="concat('::&#x9; ', */*/xmd:museum_no, ' txt&#xa;')"/>
    </xsl:if>
    <xsl:if test="string-length(*/*/xmd:accession_no) > 0">
      <xsl:value-of select="concat('::&#x9; ', */*/xmd:accession_no, ' txt&#xa;')"/>
    </xsl:if>
    <xsl:if test="string-length(*/*/xmd:excavation_no) > 0">
      <xsl:value-of select="concat('::&#x9; ', */*/xmd:excavation_no, ' txt&#xa;')"/>
    </xsl:if>
    <xsl:if test="string-length(*/*/xmd:publication_history) > 0">
      <xsl:value-of select="concat('::&#x9; ', */*/xmd:publication_history, ' txt&#xa;')"/>
    </xsl:if>
    <xsl:if test="string-length(*/*/xmd:genre) > 0">
      <xsl:value-of select="concat('::&#x9; ', */*/xmd:genre)"/>
    </xsl:if>
    <xsl:if test="string-length(*/*/xmd:subgenre) > 0">
      <xsl:value-of select="concat(' ', */*/xmd:subgenre, ' txt&#xa;')"/>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xtf:l">
<!--  <xsl:message>est-PQX xtf:l</xsl:message> -->
  <xsl:value-of select="translate(@label,' ','_')"/>
  <xsl:text>. </xsl:text>
  <xsl:apply-templates/>
<!--
  <xsl:text>&#xa;&#x9;</xsl:text>
  <xsl:apply-templates mode="hidden"/>
 -->
  <xsl:text>&#xa;</xsl:text>
  <xsl:variable name="xid" select="@xml:id"/>
  <xsl:for-each select="$translation">
    <xsl:apply-templates select="key('xtr',$xid)"/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="xh:p">
  <xsl:apply-templates/>
  <xsl:text>&#xa;</xsl:text>
<!--
  <xsl:variable name="wforms" select=".//xh:span[@xtr:form]"/>
  <xsl:if test="count($wforms)>0">
    <xsl:text>&#x9;</xsl:text>
    <xsl:for-each select="$wforms">
      <xsl:value-of select="@xtr:form"/>
      <xsl:if test="not(position()=last())">
	<xsl:text> </xsl:text>
      </xsl:if>
    </xsl:for-each>
    <xsl:text>&#xa;</xsl:text>
  </xsl:if>
 -->
</xsl:template>

<xsl:template mode="hidden" match="gdl:w">
  <xsl:call-template name="un-curly"/>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template mode="hidden" match="nrm:w">
  <xsl:call-template name="un-curly"/>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template name="un-curly">
  <xsl:param name="form" select="@form"/>
  <xsl:value-of select="$form"/>
</xsl:template>

<xsl:template name="xun-curly">
  <xsl:param name="form" select="@form"/>
  <xsl:variable name="nocurly">
    <xsl:choose>
      <xsl:when test="starts-with($form, '{')">
	<xsl:value-of select="translate(substring($form,2), '{}', '--')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="translate($form, '{}', '--')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
<!--
  <xsl:message>nocurly last char = <xsl:value-of 
  select="substring($nocurly,string-length($nocurly))"/></xsl:message>
 -->
  <xsl:choose>
    <xsl:when test="substring($nocurly,string-length($nocurly))='-'">
      <xsl:value-of select="substring($nocurly,1,string-length($nocurly)-1)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$nocurly"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:transform>