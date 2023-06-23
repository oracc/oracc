<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	       xmlns:gdf="http://oracc.org/ns/gdf/1.0"
	       version="1.0">

<xsl:output method="xml" indent="yes" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:param name="next" select="'-'"/>
<xsl:param name="prev" select="'-'"/>

<xsl:template match="gdf:entry">
  <html>
    <head>
      <link rel="stylesheet" type="text/css" href="/css/gdf.css"/>
      <title>
	<xsl:value-of select="concat(/gdf:dataset/@gdf:title, ': ', ./@gdf:title)"/>
      </title>
    </head>
    <body>
      <div class="browse">
	<span class="prev">
	  <xsl:choose>
	    <xsl:when test="string-length($prev) and not($prev='-')">
	      <a href="{/gdf:dataset/@gdf:uri-base}/{$prev}">&lt;&lt;PREV&lt;&lt;</a>
	    </xsl:when>
	    <xsl:otherwise>
	      <a class="hiding" href="#">&lt;&lt;PREV&lt;&lt;</a>
	    </xsl:otherwise>
	  </xsl:choose>
	</span>
	<span class="browse-mid">:: <a href="{/gdf:dataset/@gdf:uri-base}">Home</a> :: <a href="{/gdf:dataset/@gdf:uri-base}/find">Search</a> ::</span>
	<span class="next">
	  <xsl:choose>
	    <xsl:when test="string-length($next) and not($next='-')">
	      <a href="{/gdf:dataset/@gdf:uri-base}/{$next}">>>NEXT>></a>
	    </xsl:when>
	    <xsl:otherwise>
	      <a class="hiding" href="#">>>NEXT>></a>
	    </xsl:otherwise>
	  </xsl:choose>
	</span>
      </div>
      <h1><xsl:value-of select="concat(/gdf:dataset/@gdf:title, ': ', ./@gdf:title)"/></h1>
      <table>
	<xsl:apply-templates mode="top" select="*"/>
      </table>
    </body>
  </html>
</xsl:template>

<xsl:template mode="top" match="*">
  <tr>
    <td class="name"><xsl:value-of select="concat(local-name(), ': ')"/></td>
    <td class="data"><xsl:apply-templates mode="sub"/></td>
  </tr>
</xsl:template>

<xsl:template mode="sub" match="gdf:link">
  <a target="_blank" href="{text()}"><xsl:apply-templates/></a>
</xsl:template>

<xsl:template mode="sub" match="*">
  <xsl:apply-templates/>
  <xsl:if test="not(position()=last())">
    <xsl:text>: </xsl:text>
  </xsl:if>
</xsl:template>

</xsl:transform>