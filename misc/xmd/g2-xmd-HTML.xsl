<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns:md="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="no"/>

<xsl:template match="/">
  <xsl:variable name="name" select="/*/md:cat/md:name|/*/md:cat/md:designation"/>
  <html>
    <head>
      <link rel="stylesheet" type="text/css" href="/css/xmd.css"/>
      <title>Catalog: <xsl:value-of select="$name"/></title>
    </head>
    <body>
      <xsl:call-template name="format-xmd" />
    </body>
  </html>
</xsl:template>

<xsl:template name="format-xmd">
  <table class="xmd" >
    <xsl:choose>
      <xsl:when test="/*/md:cat/md:id_composite">
	<xsl:call-template name="format-Q"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:call-template name="format-PX"/>
      </xsl:otherwise>
    </xsl:choose>
  </table>
</xsl:template>

<xsl:template name="format-PX">
  <xsl:for-each select="/*/md:cat">
    <xsl:call-template name="field-1">
      <xsl:with-param name="nodes" select="md:id_text|md:designation|md:period|md:place|md:genre|md:subgenre"/>
    </xsl:call-template>
  </xsl:for-each>
</xsl:template>

<xsl:template name="x-format-PX">
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Primary Publication 
      </td>
      <td class="xmd" width="500">
	<xsl:if test="string-length(/*/md:cat/md:primary_publication) > 0">
	  <xsl:value-of select="/*/md:cat/md:primary_publication"
			/><xsl:text>, </xsl:text><xsl:value-of select="/*/md:cat/md:author"
					  /><xsl:if test="string-length(/*/md:cat/md:publication_date) > 0">,
	    <xsl:value-of select="/*/md:cat/md:publication_date"/></xsl:if>
	</xsl:if>
	<xsl:if test="string-length(/*/md:cat/md:pubelectronic) > 0">
	  <a href="{/*/md:cat/md:pubelectronic}" target="blank"
	     >&#xa0;[<span style="font-variant:small-caps" >link</span>]</a>
	</xsl:if>
      </td>
    </tr>
    <xsl:if test="string-length(/*/md:cat/md:publication_history)">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Publication history 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:publication_history"/>
	</td>
      </tr>
    </xsl:if>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Collection 
      </td>
      <!-- COLLECTION INFORMATION HAS TO BE ADDED, HERE USED OWNER -->
      <td class="xmd" width="500">
	<xsl:value-of select="/*/md:cat/md:owner"/>
      </td>
    </tr>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Museum no. 
      </td>
      <td class="xmd" width="500">
	<xsl:value-of select="/*/md:cat/md:museum_no"/>
      </td>
    </tr>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Excavation no. 
      </td>
      <td class="xmd" width="500">
	<xsl:value-of select="/*/md:cat/md:excavation_no"/>
      </td>
    </tr>
    <xsl:if test="string-length(/*/md:cat/md:provenience) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Provenience 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:provenience"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:cat/md:genre) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Genre 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:genre"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:cat/md:subgenre) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Subgenre 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:subgenre"/>
	</td>
      </tr>
    </xsl:if>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Period 
      </td>
      <td class="xmd" width="500">
	<xsl:value-of select="/*/md:cat/md:period"/>
      </td>
    </tr>
    <xsl:if test="string-length(/*/md:cat/md:dateoforigin) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Date of Origin 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:dateoforigin"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:cat/md:display_size) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Measurements (mm)
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:display_size"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:cat/md:sealinfo) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Seal information
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:sealinfo"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:cat/md:displaysize) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Measurements (mm)
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:displaysize"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:cat/md:authorremarks) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Author remarks
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:authorremarks"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:cat/md:collation) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Collation
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:cat/md:collation"/>
	</td>
      </tr>
    </xsl:if>
</xsl:template>

<xsl:template name="format-Q">
  <xsl:for-each select="/*/md:cat">
    <xsl:call-template name="field-1">
      <xsl:with-param name="nodes" select="md:id_composite|md:designation|md:period|md:place|md:genre|md:subgenre"/>
    </xsl:call-template>
  </xsl:for-each>
</xsl:template>

<xsl:template name="field-1">
  <xsl:param name="nodes"/>
  <xsl:for-each select="$nodes">
    <tr>
      <td class="field-1-name"><xsl:value-of select="local-name(.)"/></td>
      <td class="field-2-text"><xsl:value-of select="."/></td>
    </tr>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
