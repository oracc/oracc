<xsl:stylesheet 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:xmd="http://oracc.org/ns/xmd/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns="http://www.tei-c.org/ns/1.0"
    extension-element-prefixes="ex"
    >

<xsl:param name="one-big-file" select="'no'"/>
<xsl:output method="xml" indent="yes" encoding="UTF-8"/>

<xsl:template name="langtags">
  <tag l="Akkadian">akk</tag>
  <tag l="Hittite">hit</tag>
  <tag l="Sumerian">sux</tag>
  <tag l="Ugaritic">uga</tag>
</xsl:template>

<xsl:template match="xmd-set">
  <xsl:choose>
    <xsl:when test="$one-big-file = 'yes'">
      <fake-tei-wrapper>
	<xsl:apply-templates/>
      </fake-tei-wrapper>
    </xsl:when>
    <xsl:otherwise>
      <xsl:for-each select="xmd:xmd">
	<ex:document href="01tmp/teihdrs/{@xml:id}-hdr.xml">
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
  <teiHeader>
    <fileDesc>
      <titleStmt>
	<title><xsl:value-of select="$xmd/xmd:designation"/>; <xsl:value-of select="$xmd/xmd:join_information"/></title>
	<editor><xsl:value-of select="$xmd/xmd:editor"/></editor>
	<funder><xsl:value-of select="$xmd/xmd:funder"/></funder>
	<principal><xsl:value-of select="$xmd/xmd:principal"/></principal>
      </titleStmt>
      <publicationStmt>
	<distributor><xsl:value-of select="$xmd/xmd:repository"/></distributor>
	<idno type="cdli-P-number"><xsl:value-of select="$xmd/xmd:id_text"/></idno>
	<availability>
	  <licence target="http://creativecommons.org/licenses/by-sa/3.0/us/">
	    <p>This work is licensed under Creative Commons Attribution-ShareAlike 3.0 United States (CC BY-SA 3.0 US)</p>
	    <p>You are free: to Share — to copy, distribute and transmit the work; to Remix — to adapt the work; to make commercial use of the work.</p>
	    <p>Under the following conditions:  Attribution — You must attribute the work in the manner specified by the author or licensor (but not in any way that suggests that they endorse you or your use of the work); Share Alike — If you alter, transform, or build upon this work, you may distribute the resulting work only under the same or similar license to this one.</p>
	    <p>For more information please visit http://creativecommons.org/licenses/by-sa/3.0/us/</p>
	  </licence>
	</availability>
	<date><xsl:value-of select="$xmd/xmd:uploaded"/></date>
      </publicationStmt>
      <sourceDesc>
	<msDesc>
	  <msIdentifier>
	    <institution><xsl:value-of select="$xmd/xmd:owner"/></institution>
	    <idno type="museum"><xsl:value-of select="$xmd/xmd:museum_no"/></idno>
	    <altIdentifier type="accession"><idno><xsl:value-of select="$xmd/xmd:accession_no"/></idno></altIdentifier>
	    <altIdentifier type="excavation"><idno><xsl:value-of select="$xmd/xmd:excavation_no"/></idno></altIdentifier>
	  </msIdentifier>
	  <physDesc>
	    <objectDesc>
	      <p>
		<objectType><xsl:value-of select="$xmd/xmd:object_type"/></objectType>
		<material><xsl:value-of select="$xmd/xmd:material"/></material>
		<dimensions>
		  <height><xsl:value-of select="$xmd/xmd:height"/></height>
		  <width><xsl:value-of select="$xmd/xmd:width"/></width>
		  <depth><xsl:value-of select="$xmd/xmd:thickness"/></depth>
		</dimensions>
	      </p>
	    </objectDesc>
	  </physDesc>
	  <history>
	    <p>
	      <origDate><xsl:value-of select="$xmd/xmd:period"/></origDate>
	      <origDate><xsl:value-of select="$xmd/xmd:period_remarks"/></origDate>
	      <origDate><xsl:value-of select="$xmd/xmd:date_of_origin"/></origDate>
	      <origDate><xsl:value-of select="$xmd/xmd:date_remarks"/></origDate>
	    </p>
	  </history>
	  <origin>
	    <origPlace><xsl:value-of select="$xmd/xmd:provenience"/></origPlace>
	    <origPlace><xsl:value-of select="$xmd/xmd:provenience_remarks"/></origPlace>
	    <origPlace><xsl:value-of select="$xmd/xmd:findspot_square"/></origPlace>
	    <origPlace><xsl:value-of select="$xmd/xmd:stratigraphic_level"/></origPlace>
	  </origin>
	</msDesc>
	<listBibl>
	  <bibl><xsl:value-of select="$xmd/xmd:author"/>, <xsl:value-of select="$xmd/xmd:primary_publication"/>, <xsl:value-of select="$xmd/xmd:publication_date"/></bibl>
	</listBibl>
      </sourceDesc>
    </fileDesc>
    <encodingDesc>
      <projectDesc><p>Created for the Oracc project <xsl:value-of select="$xmd/xmd:project"/></p></projectDesc>
    </encodingDesc>
    <profileDesc>
      <creation>
	<xsl:value-of select="$xmd/xmd:credits"/>
      </creation>
      <xsl:if test="string-length($xmd/xmd:language)>0">
	<xsl:variable name="ident-lang">
	  <xsl:choose>
	    <xsl:when test="contains($xmd/xmd:language, 'lingual')">
	      <xsl:choose>
		<xsl:when test="contains($xmd/xmd:language, '/')">
		  <xsl:value-of select="substring-before($xmd/xmd:language, '/')"/>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:message>tei-xmd-headers.xsl: no / in xxxlingual language</xsl:message>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$xmd/xmd:language"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="type-lang">
	  <xsl:choose>
	    <xsl:when test="contains($xmd/xmd:language, 'lingual')">
	      <xsl:value-of select="$xmd/xmd:language"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<langUsage>
	  <language>
	    <xsl:attribute name="ident">
	      <xsl:choose>
		<xsl:when test="$ident-lang='Akkadian'">
		  <xsl:text>akk</xsl:text>
		</xsl:when>
		<xsl:when test="$ident-lang='Hittite'">
		  <xsl:text>hit</xsl:text>
		</xsl:when>
		<xsl:when test="$ident-lang='Sumerian'">
		  <xsl:text>sux</xsl:text>
		</xsl:when>
		<xsl:when test="$ident-lang='Ugaritic'">
		  <xsl:text>uga</xsl:text>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:message>unhandled language: <xsl:value-of select="$xmd/xmd:language"/></xsl:message>
		  <xsl:text>zzz</xsl:text>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:attribute>
	    <xsl:if test="string-length($type-lang)>0">
	      <xsl:attribute name="type"><xsl:value-of select="$type-lang"/></xsl:attribute>
	    </xsl:if>
	    <xsl:value-of select="$xmd/xmd:language"/>
	  </language>
	</langUsage>
      </xsl:if>
      <textClass>
	<keywords scheme="oracc">
	  <term><xsl:value-of select="$xmd/xmd:genre"/></term>
	  <term><xsl:value-of select="$xmd/xmd:subgenre"/></term>
	  <term><xsl:value-of select="$xmd/xmd:subgenre_remarks"/></term>
	</keywords>
      </textClass>
    </profileDesc>
  </teiHeader>
</xsl:template>
</xsl:stylesheet>
