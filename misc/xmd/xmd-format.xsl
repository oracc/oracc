<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns:md="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="xml" indent="no"/>

<xsl:template name="xmd-format" >
  <table class="xmd" >
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Primary Publication 
      </td>
      <td class="xmd" width="500">
	<xsl:if test="string-length(/*/md:primary_publication) > 0">
	  <xsl:value-of select="/*/md:primary_publication"
			/><xsl:text>, </xsl:text><xsl:value-of select="/*/md:author"
					  /><xsl:if test="string-length(/*/md:publication_date) > 0">,
	    <xsl:value-of select="/*/md:publication_date"/></xsl:if>
	</xsl:if>
	<xsl:if test="string-length(/*/md:pubelectronic) > 0">
	  <a href="{/*/md:pubelectronic}" target="blank"
	     >&#xa0;[<span style="font-variant:small-caps" >link</span>]</a>
	</xsl:if>
      </td>
    </tr>
    <xsl:if test="string-length(/*/md:publication_history)">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Publication history 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:publication_history"/>
	</td>
      </tr>
    </xsl:if>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Collection 
      </td>
      <!-- COLLECTION INFORMATION HAS TO BE ADDED, HERE USED OWNER -->
      <td class="xmd" width="500">
	<xsl:value-of select="/*/md:owner"/>
      </td>
    </tr>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Museum no. 
      </td>
      <td class="xmd" width="500">
	<xsl:value-of select="/*/md:museum_no"/>
      </td>
    </tr>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Excavation no. 
      </td>
      <td class="xmd" width="500">
	<xsl:value-of select="/*/md:excavation_no"/>
      </td>
    </tr>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	ORACCI no. 
      </td>
      <td class="xmd" width="500">
	<xsl:value-of select="/*/@xml:id"/>
      </td>
    </tr>
    <xsl:if test="string-length(/*/md:provenience) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Provenience 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:provenience"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:genre) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Genre 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:genre"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:subgenre) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Subgenre 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:subgenre"/>
	</td>
      </tr>
    </xsl:if>
    <tr align="left" valign="top">
      <td class="xmd" width="150" align="left">
	Period 
      </td>
      <td class="xmd" width="500">
	<xsl:value-of select="/*/md:period"/>
      </td>
    </tr>
    <xsl:if test="string-length(/*/md:dateoforigin) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Date of Origin 
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:dateoforigin"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:display_size) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Measurements (mm)
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:display_size"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:sealinfo) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Seal information
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:sealinfo"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:displaysize) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Measurements (mm)
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:displaysize"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:authorremarks) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Author remarks
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:authorremarks"/>
	</td>
      </tr>
    </xsl:if>
    <xsl:if test="string-length(/*/md:collation) > 0">
      <tr align="left" valign="top">
	<td class="xmd" width="150" align="left">
	  Collation
	</td>
	<td class="xmd" width="500">
	  <xsl:value-of select="/*/md:collation"/>
	</td>
      </tr>
    </xsl:if>
  </table>
</xsl:template>

</xsl:stylesheet>
