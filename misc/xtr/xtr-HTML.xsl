<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xh xtr">

<xsl:param name="mode" select="'www'"/>
<xsl:param name="project" select="''"/>
<xsl:param name="access" select="''"/>
<xsl:param name="line-id" select="''"/>
<xsl:param name="frag-id" select="''"/>
<xsl:param name="render-tlat-style" select="'#none'"/>

<xsl:include href="html-standard.xsl"/>

<xsl:output method="xml" indent="no" encoding="utf-8"/>

<xsl:template match="xtr:translation">
  <xsl:variable name="css-project" select="$project"/>
  <html>
    <head>
      <link rel="stylesheet" type="text/css" 
	    href="{concat('/',$css-project,'.css')}"/>
      <script src="/oracc.js" type="text/javascript">
	<xsl:text> </xsl:text>
      </script>
      <title>
	<xsl:value-of select="concat('Translation of ',
			             translate(/xtr:translation/@n,'ḫ','h'))"/>
      </title>
    </head>
    <body onload="window.location.hash='a.{$frag-id}'">
      <h1 class="tr title">
	<xsl:text>Translation of </xsl:text>
	<xsl:value-of select="translate(/xtr:translation/@n,'ḫ','h')"/>
      </h1>
      <xsl:apply-templates/>
    </body>
  </html>
</xsl:template>

<xsl:template match="xh:p[@class='tr']">
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@class"/>
<!--
    <a href="{@xtr:sref}" name="{@xml:id}">
      <xsl:value-of select="@xtr:label"/>
      <xsl:text>.&#xa0;</xsl:text>
    </a>
 -->
    <span class="label">
      <xsl:choose>
	<xsl:when test="$render-tlat-style='rinap'">
	  <xsl:message>render-tlat-style=rinap</xsl:message>
	  <xsl:value-of select="@xtr:lab-start-label"/>
	  <xsl:if test="string-length(@xtr:lab-end-label)>0">
	    <xsl:value-of select="concat('-',@xtr:lab-end-label)"/>
	  </xsl:if>
	  <xsl:text>)&#xa0;</xsl:text>  
	</xsl:when>
	<xsl:otherwise>
	  <xsl:message>render-tlat-style=none</xsl:message>
	  <xsl:choose>
	    <xsl:when test="@xtr:disp">
	      <xsl:value-of select="@xtr:disp"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="@xtr:label"/>
	    </xsl:otherwise>
	  </xsl:choose>
	  <xsl:text>.&#xa0;</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </span>
    <xsl:apply-templates/>
  </xsl:element>
</xsl:template>

<xsl:template match="xh:innerp">
  <xsl:apply-templates/>
  <xsl:if test="following-sibling::xh:innerp">
    <br/><span class="indent"/>
  </xsl:if>
</xsl:template>

<xsl:template match="xh:innerp[@class='tr-comment']"/>

<xsl:template match="xh:span[@class='marker']">
  <a href="javascript://" onmouseover="showNote(event,'tnote{text()}')">
    <xsl:element name="{local-name()}">
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:element>
  </a>
</xsl:template>

<xsl:template match="xh:span[@class='w']">
  <xsl:copy>
    <xsl:copy-of select="@*[not(name()='xml:id')]"/>
    <xsl:choose>
      <xsl:when test="substring(@xml:id,string-length(@xml:id)-1) = '.0'">
	<a name="{substring-before(@xml:id,'.0')}">
	  <xsl:apply-templates/>
	</a>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:copy>
</xsl:template>

<xsl:template mode="maybe-with-note-marker" match="xh:td">
  <xsl:variable name="marker">
    <xsl:if test="../following-sibling::*[1][@class='note']">
      <xsl:value-of select="1+count(../preceding-sibling::*[@class='note'])"/>
    </xsl:if>
  </xsl:variable>
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:choose>
      <xsl:when test="xh:p"> <!-- we assume only 1 p within <td> -->
	<xsl:for-each select="xh:p">
	  <xsl:copy>
	    <xsl:copy-of select="@*"/>
	    <xsl:apply-templates/>
	    <xsl:if test="string-length($marker) > 0">
	      <a href="javascript://" onmouseover="showNote(event,'{concat('tnote',$marker)}')">
		<span class="marker"><xsl:value-of select="$marker"/></span>
	      </a>
	    </xsl:if>
	  </xsl:copy>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:apply-templates/>
	<xsl:if test="string-length($marker) > 0">
	  <a href="javascript://" onmouseover="showNote(event,'{concat('tnote',$marker)}')">
	    <span class="marker"><xsl:value-of select="$marker"/></span>
	  </a>
	</xsl:if>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:copy>
</xsl:template>

<xsl:template match="xh:p[@class='note']"/>

<xsl:template mode="notedump" match="xh:p[@class='note']">
</xsl:template>

<xsl:template match="xh:*">
  <xsl:param name="xtr"/>
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates>
      <xsl:with-param name="xtr" select="$xtr"/>
    </xsl:apply-templates>
  </xsl:element>
</xsl:template>

<xsl:template mode="notediv" match="xh:*">
  <xsl:param name="xtr"/>
  <xsl:element name="{local-name()}">
    <xsl:copy-of select="@*[not(local-name(.)='id')]"/>
    <xsl:apply-templates mode="notediv">
      <xsl:with-param name="xtr" select="$xtr"/>
    </xsl:apply-templates>
  </xsl:element>
</xsl:template>

<xsl:template match="xtr:*"/>

</xsl:stylesheet>
