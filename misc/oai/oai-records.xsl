<?xml version="1.0" encoding="UTF-8"?>
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:xpd="http://oracc.org/ns/xpd/1.0"
	       version="1.0">

<xsl:template match="/projects">
  <oai-records>
    <xsl:apply-templates/>
  </oai-records>
</xsl:template>

<xsl:template match="xpd:project">
  <oai-record id="{@n}" date="{@date}">
    <record>
      <header>
	<identifier><xsl:value-of select="@n"/></identifier>
	<datestamp><xsl:value-of select="@date"/></datestamp>
      </header>
      <metadata>
	<oai_dc:dc 
            xmlns:oai_dc="http://www.openarchives.org/OAI/2.0/oai_dc/" 
            xmlns:dc="http://purl.org/dc/elements/1.1/" 
            xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
            xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/oai_dc/ http://www.openarchives.org/OAI/2.0/oai_dc.xsd">
				<dc:title><xsl:value-of select="xpd:name"/></dc:title>
				<dc:creator>The Oracc project: <xsl:value-of select="xpd:name"/></dc:creator>
				<dc:type>text</dc:type>
				<dc:description><xsl:apply-templates select="xpd:blurb"/></dc:description>
				<dc:identifier>http://oracc.org/<xsl:value-of select="@n"/></dc:identifier>
	</oai_dc:dc>
      </metadata>
    </record>
  </oai-record>
</xsl:template>

<xsl:template match="*">
  <xsl:apply-templates/>
</xsl:template>

</xsl:transform>
