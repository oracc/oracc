<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:key name="fuzzy-key1" match="name" use="substring(@key,1,string-length(@key)-1)"/>
<xsl:key name="fuzzy-key2" match="name" use="substring(@key,1,string-length(@key)-2)"/>
<xsl:key name="fuzzy-key3" match="name" use="substring(@key,1,string-length(@key)-3)"/>

<xsl:variable name="lc" select="'abcdefghijklmnopqrstuvwxyz'"/>

<xsl:template name="map-name-fuzzy">
  <xsl:param name="nm" select="text()"/>
  <xsl:variable name="name" select="normalize-space($nm)"/>
  <xsl:if test="string-length($name) > 0">
    <xsl:variable name="incoming-key">
      <xsl:call-template name="make-fuzzy-key">
	<xsl:with-param name="nm2" select="$name"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:if test="string-length($incoming-key)>0">
      <xsl:for-each select="document('/usr/local//lib/bib/names.xml')">
	<xsl:variable name="key1" select="key('fuzzy-key1',$incoming-key)"/>
	<xsl:choose>
	  <xsl:when test="$key1">
	    <xsl:value-of select="$key1/@key"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:variable name="key2" select="key('fuzzy-key2',$incoming-key)"/>
	    <xsl:choose>
	      <xsl:when test="$key2">
		<xsl:value-of select="$key2/@key"/>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:variable name="key3" select="key('fuzzy-key3',$incoming-key)"/>
		<xsl:choose>
		  <xsl:when test="$key3">
		    <xsl:value-of select="$key3/@key"/>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:value-of select="''"/>
		  </xsl:otherwise>
		</xsl:choose>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:for-each>
    </xsl:if>
  </xsl:if>
</xsl:template>

<xsl:template name="make-fuzzy-key">
  <xsl:param name="nm" select="text()"/>
  <xsl:choose>
    <xsl:when test="contains($nm,', ')">
      <xsl:variable name="last" select="substring-before($nm,', ')"/>
      <xsl:variable name="first" select="substring-after($nm,', ')"/>
      <xsl:value-of select="concat($last,substring(substring-after($nm,', '),1,1))"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="''"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
