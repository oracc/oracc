<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns:o="http://oracc.org/ns/ood/1.0"
	       xmlns="http://www.w3.org/1999/xhtml"
	       exclude-result-prefixes="o">

<xsl:param name="html" select="'no'"/>
<xsl:param name="project" select="/*/@o:project"/>

<xsl:output method="xml" indent="yes" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:template match="o:records">
  <xsl:choose>
    <xsl:when test="$html='yes'">
      <html>
	<head>
	  <link rel="stylesheet" type="text/css" href="/css/ood.css"/>
	  <title><xsl:value-of select="$project"/>/<xsl:value-of select="o:id"/></title>
	</head>
	<body>
	  <xsl:apply-templates/>
	</body>
      </html>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="o:record">
  <div class="ood-record">
    <h1>http://oracc.org/<xsl:value-of select="$project"/>/<xsl:value-of select="o:id"/> 
    = <xsl:value-of select="o:n"/></h1>
    <table>
      <xsl:for-each select="*[not(self::o:*)]">
	<tr>
	  <td class="ood-field"><xsl:value-of select="local-name(.)"/>:</td>
	  <td class="ood-value"><xsl:value-of select="text()"/>
	  </td>
	</tr>
      </xsl:for-each>
    </table>
  </div>
</xsl:template>

</xsl:transform>
