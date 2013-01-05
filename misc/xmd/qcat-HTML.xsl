<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns:md="http://oracc.org/ns/xmd/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="md">

<xsl:output method="xml" indent="no"/>

<xsl:template match="/">
  <xsl:for-each select="/*/md:cat">
    <xsl:call-template name="xmd-format-q"/>
  </xsl:for-each>
</xsl:template>

<xsl:template name="xmd-format-q" >
  <div class="qdata">
    <h1>Q Catalogue entry for <xsl:value-of 
          select="md:id_composite"/> = <xsl:value-of 
	  select="md:designation"/>
    </h1>

    <xsl:if test="string-length(md:other_names)>0">
      <xsl:call-template name="field">
	<xsl:with-param name="node" select="md:other_names"/>
      </xsl:call-template>
    </xsl:if>

    <table>
      <tr>
	<td>
	  <xsl:call-template name="field">
	    <xsl:with-param name="node" select="md:period"/>
	  </xsl:call-template>
	</td>
	<td>
	  <xsl:call-template name="field">
	    <xsl:with-param name="node" select="md:genre"/>
	  </xsl:call-template>
	</td>
      </tr>
      <tr>
	<td>
	  <xsl:call-template name="field">
	    <xsl:with-param name="node" select="md:place"/>
	  </xsl:call-template>
	</td>
	<td>
	  <xsl:call-template name="field">
	    <xsl:with-param name="node" select="md:subgenre"/>
	  </xsl:call-template>
	</td>
      </tr>
      <tr>
	<td>
	  <xsl:call-template name="field">
	    <xsl:with-param name="node" select="md:series"/>
	  </xsl:call-template>
	</td>
      </tr>
      <tr>
	<td>
	  <xsl:call-template name="field">
	    <xsl:with-param name="node" select="md:subseries"/>
	  </xsl:call-template>
	</td>
      </tr>
    </table>

    <xsl:call-template name="field">
      <xsl:with-param name="node" select="md:primary_edition"/>
    </xsl:call-template>
    
    <xsl:call-template name="field">
      <xsl:with-param name="node" select="md:publication_history"/>
    </xsl:call-template>
    
    <xsl:call-template name="field">
      <xsl:with-param name="node" select="md:keywords"/>
    </xsl:call-template>

    <p><xsl:apply-templates select="md:description"/></p>

    <!-- SOURCES ARE GOING TO COME IN HERE -->
  </div>
</xsl:template>

<xsl:template name="field">
  <xsl:param name="node"/>
  <p>
    <span class="fieldname"><xsl:value-of select="local-name($node)"/>: </span>
    <xsl:value-of select="$node"/>
  </p>
</xsl:template>

</xsl:stylesheet>
