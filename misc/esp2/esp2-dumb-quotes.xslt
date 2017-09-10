<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">
   <xsl:character-map name="dumb-quotes">
		<xsl:output-character character="”" string='"'/>
		<xsl:output-character character="“" string='"'/>
		<xsl:output-character character="‘" string="'"/>
		<xsl:output-character character="’" string="'"/>
		<xsl:output-character character="…" string="..."/>
<!--		<xsl:output-character character="–" string="-"/>
		<xsl:output-character character="—" string=" - "/> -->
   </xsl:character-map>
</xsl:stylesheet>
