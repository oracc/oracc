<?xml version='1.0'?>

<xsl:stylesheet version="1.0"
  xmlns="http://oracc.org/ns/xix/1.0"
  xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
  xmlns:cbd="http://oracc.org/ns/cbd/1.0"
  xmlns:epad="http://psd.museum.upenn.edu/epad/"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="no" encoding="utf-8" omit-xml-declaration="no"/>

<xsl:template name="make-index">
  <xsl:param name="title"/>
  <xsl:param name="basename"/>
  <xsl:param name="node-list"/>
  <xsl:message>make-index passed node-list with <xsl:value-of 
	select="count($node-list)"/> entries</xsl:message>
  <index xml:base="'http://oracc.museum.upenn.edu/'" title="{$title}" basename="{$basename}">
    <xsl:for-each select="$node-list">
      <xsl:variable name="xid">
	<xsl:choose>
	  <xsl:when test="@entry">
	    <xsl:value-of select="@entry"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="ancestor-or-self::cbd:entry/@xml:id"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:variable>
      <ix>
   	<xsl:attribute name="sortkey">
          <xsl:call-template name="make-sortkey"/>
        </xsl:attribute>
        <what>
          <xsl:call-template name="make-what"/>
  	</what>
	<where>
          <xsl:attribute name="ref">
	    <xsl:value-of select="$xid"/>
	  </xsl:attribute>
         <xsl:call-template name="make-where"/>
 	</where>
      </ix>
    </xsl:for-each>
  </index>
</xsl:template>

</xsl:stylesheet>
