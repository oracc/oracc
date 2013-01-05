<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xmd="http://oracc.org/ns/xmd/1.0"
    xmlns:ex="http://exslt.org/common"
    extension-element-prefixes="ex"
    >

<xsl:param name="one-big-file" select="'no'"/>
<xsl:output method="xml" indent="yes" encoding="UTF-8"/>

<xsl:template match="xmd-set">
  <xsl:choose>
    <xsl:when test="$one-big-file = 'yes'">
      <fake-tei-wrapper>
	<xsl:apply-templates/>
      </fake-tei-wrapper>
    </xsl:when>
    <xsl:otherwise>
      <xsl:for-each select="xmd:xmd">
	<ex:document href="02xml/00tei/{@xml:id}-hdr.xml">
	  <xsl:call-template name="TEIHeader">
	    <xsl:with-param name="xmd" select="xmd:cat"/>
	  </xsl:call-template>
	</ex:document>
      </xsl:for-each>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="xmd:xmd">
  <xsl:for-each select="xmd:cat">
    <xsl:call-template name="TEIHeader"/>
  </xsl:for-each>
</xsl:template>

<xsl:template name="TEIHeader">
  <xsl:param name="xmd" select="."/>
  <TEIHeader>
    <fileDesc>
      <titleStmt>
	<title><xsl:value-of select="$xmd/xmd:designation"/>; <xsl:value-of select="$xmd/xmd:join_information"/></title>
	<editor><xsl:value-of select="$xmd/xmd:editor"/></editor>
	<funder><xsl:value-of select="$xmd/xmd:funder"/></funder>
	<principal><xsl:value-of select="$xmd/xmd:principal"/></principal>
      </titleStmt>
      <publicationStmt>
	<distributor><xsl:value-of select="$xmd/xmd:repository"/></distributor>
	<idno type="XXX"><xsl:value-of select="$xmd/xmd:id_text"/></idno>
	<availability><xsl:value-of select="$xmd/xmd:credits"/></availability>
	<date><xsl:value-of select="$xmd/xmd:uploaded"/></date>
      </publicationStmt>
      <sourceDesc>
	<msDesc>
	  <msIdentifier>
	    <institution><xsl:value-of select="$xmd/xmd:owner"/></institution>
	    <idno><xsl:value-of select="$xmd/xmd:museum_no"/></idno>
	    <altIdentifier> <xsl:value-of select="$xmd/xmd:accession_no"/></altIdentifier>
	  </msIdentifier>
	</msDesc>
	<listBibl>
	  <bibl><xsl:value-of select="$xmd/xmd:author"/>, <xsl:value-of select="$xmd/xmd:primary_publication"/>, <xsl:value-of select="$xmd/xmd:publication_date"/></bibl>
	</listBibl>
      </sourceDesc>
      <encodingDesc>
	<projectDesc><xsl:value-of select="$xmd/xmd:project"/></projectDesc>
      </encodingDesc>
      <profileDesc>
	<creation>
	  <objectType><xsl:value-of select="$xmd/xmd:object_type"/></objectType>
	  <material><xsl:value-of select="$xmd/xmd:material"/></material>
	  <height><xsl:value-of select="$xmd/xmd:height"/></height>
	  <width><xsl:value-of select="$xmd/xmd:width"/></width>
	  <depth><xsl:value-of select="$xmd/xmd:thickness"/></depth>
	  <origDate><xsl:value-of select="$xmd/xmd:period"/></origDate>
	  <origDate><xsl:value-of select="$xmd/xmd:period_remarks"/></origDate>
	  <origDate><xsl:value-of select="$xmd/xmd:date_of_origin"/></origDate>
	  <origDate><xsl:value-of select="$xmd/xmd:date_remarks"/></origDate>
	  <origPlace><xsl:value-of select="$xmd/xmd:provenience"/></origPlace>
	  <origPlace><xsl:value-of select="$xmd/xmd:provenience_remarks"/></origPlace>
	  <origPlace><xsl:value-of select="$xmd/xmd:findspot_square"/></origPlace>
	  <origPlace><xsl:value-of select="$xmd/xmd:stratigraphic_level"/></origPlace>
	  <idno><xsl:value-of select="$xmd/xmd:excavation_no"/></idno>
	</creation>
	<langUsage>
	  <language><xsl:value-of select="$xmd/xmd:language"/></language>
	</langUsage>
	<textClass>
	  <keywords>
	    <term><xsl:value-of select="$xmd/xmd:genre"/></term>
	    <term><xsl:value-of select="$xmd/xmd:subgenre"/></term>
	    <term><xsl:value-of select="$xmd/xmd:subgenre_remarks"/></term>
	  </keywords>
	</textClass>
      </profileDesc>
    </fileDesc>
  </TEIHeader>
</xsl:template>
</xsl:stylesheet>
