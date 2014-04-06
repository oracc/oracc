<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
    xmlns:gdf="http://oracc.org/ns/gdf/1.0"
    >
<xsl:param name="abbrev" select="''"/>
<xsl:param name="id-base" select="''"/>
<xsl:param name="title" select="''"/>
<xsl:param name="uri-base" select="''"/>

<xsl:output method="xml" encoding="utf-8" indent="no"/>

<xsl:template match="/">
  <xsl:choose>
    <xsl:when test="string-length($abbrev)=0">
      <xsl:message>xml2gdf.xsl: must set string-param abbrev</xsl:message>
    </xsl:when>
    <xsl:when test="string-length($title)=0">
      <xsl:message>xml2gdf.xsl: must set string-param title</xsl:message>
    </xsl:when>
    <xsl:when test="string-length($uri-base)=0">
      <xsl:message>xml2gdf.xsl: must set string-param uri-base</xsl:message>
    </xsl:when>
    <xsl:otherwise>
      <gdf:dataset gdf:title="{$title}" gdf:uri-base="{$uri-base}" gdf:abbrev="{$abbrev}">
	<xsl:choose>
	  <xsl:when test="string-length($id-base)>0">
	    <xsl:variable name="last" select="1+count(*/*)"/>
	    <xsl:variable name="format" select="concat(
						  substring(
						    translate($last,'123456789','000000000'),
						    1,
						    string-length($last)-1
						    ),
						  '1'
						)"/>
	    <xsl:for-each select="*/*">
	      <gdf:entry>
		<xsl:variable name="nth">
		  <xsl:number value="count(preceding-sibling::*)+1" format="{$format}"/>
		</xsl:variable>
		<xsl:attribute name="xml:id">
		  <xsl:value-of select="concat($id-base, '.', $nth)"/>
		</xsl:attribute>
		<xsl:apply-templates select="*"/>
	      </gdf:entry>
	    </xsl:for-each>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:for-each select="*/*">
	      <gdf:entry>
		<xsl:apply-templates select="*"/>
	      </gdf:entry>
	    </xsl:for-each>
	  </xsl:otherwise>
	</xsl:choose>
      </gdf:dataset>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="*">
  <xsl:element name="{local-name(.)}">
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

</xsl:transform>
