<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:gdl="http://oracc.org/ns/gdl/1.0"
		xmlns:norm="http://oracc.org/ns/norm/1.0"
		xmlns:nsa="http://oracc.org/ns/nsa/1.0"
		xmlns:xcl="http://oracc.org/ns/xcl/1.0"
		xmlns:xff="http://oracc.org/ns/xff/1.0"
		xmlns:xl="http://www.w3.org/1999/xlink"
		xmlns:xtf="http://oracc.org/ns/xtf/1.0"
		xmlns:tei="http://www.tei-c.org/ns/1.0"
		xmlns="http://www.tei-c.org/ns/1.0"
		exclude-result-prefixes="gdl nsa xcl xff xl xtf"
		>

<!-- @SUMMARY@= Performs heavy lifting of XTF to TEI text transformations. -->

<xsl:param name="version" select="'1.1'"/>

<xsl:param name="project"/>

<xsl:template match="xmd:xmd-set">
  <teiHeader>
    <fileDesc>
      <titleStmt>
	<title>
	  <note type="helplink" 
		xl:type="simple" 
		xl:href="http://oracc.museum.upenn.edu/doc/user/teicolours.html" 
		xl:show="new">(A key to the colourization may be found at: 
	  http://oracc.museum.upenn.edu/doc/user/teicolours.html.)</note>
	  <name xl:type="simple" xl:href="http://cdli.ucla.edu/{@xml:id}" 
		xl:show="new" type="cdlicat:id_text"><xsl:value-of select="@xml:id"/></name>
	  <xsl:text> = </xsl:text>
	  <name type="cdlicat:primary_publication"><xsl:value-of select="@n"/></name>
	</title>
	<respStmt>
	  <resp>generated at <date><xsl:value-of select="$datestamp"/></date> by </resp>
	  <name type="program">xtf2tei.xsl 1.1</name>
	</respStmt>
      </titleStmt>
      <publicationStmt>
	<availability><p>Electronic manuscript</p></availability>
      </publicationStmt>
      <sourceDesc>
	<bibl><name type="file"><xsl:value-of select="$source"/></name></bibl>
      </sourceDesc>
    </fileDesc>
  </teiHeader>
</xsl:template>

</xsl:stylesheet>