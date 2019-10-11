<?xml version="1.0" encoding="UTF-8"?>
<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSLT/Transform" version="1.0">
<xsl:param name="date"/>
<xsl:param name="referer"/>
<xsl:template match="/">
  <OAI-PMH xmlns="http://www.openarchives.org/OAI/2.0/"
	 xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	 xsi:schemaLocation="http://www.openarchives.org/OAI/2.0/
			     http://www.openarchives.org/OAI/2.0/OAI-PMH.xsd">
  <responseDate><xsl:value-of select="$date"/></responseDate>
  <request verb="ListMetadataFormats">
  <xsl:value-of select="$referer"/></request>
  <ListMetadataFormats>
    <metadataFormat>
      <metadataPrefix>oai_dc</metadataPrefix>
      <schema>http://www.openarchives.org/OAI/2.0/oai_dc.xsd</schema>
      <metadataNamespace>http://www.openarchives.org/OAI/2.0/oai_dc/
      </metadataNamespace>
    </metadataFormat>
  </ListMetadataFormats>
  </OAI-PMH>
</xsl:template>
</xsl:transform>
