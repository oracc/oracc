<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="html-standard.xsl"/>

<xsl:output method="xml" encoding="utf-8" omit-xml-declaration="yes"/>

<xsl:param name="formset" select="/*/@formset"/>
<xsl:param name="project"/>

<xsl:template name="columns">
  <col th="Form" attr="form" sortable="yes"/>
<!--  <col th="CFGW" attr="cfgw" sortable="yes"/> -->
<!--  <col th="POS"  attr="pos"  sortable="yes" reversible="yes"/> -->
  <col th="Base" attr="base" sortable="yes"/>
  <col th="Morph" attr="morph" sortable="yes"/>
  <col th="Freq" attr="freq" sortable="yes" reversible="yes"/>
</xsl:template>

<xsl:template match="/">
  <html>
    <head>
      <title>ePSD Forms: <xsl:value-of select="$formset"/></title>
      <link rel="stylesheet" type="text/css" href="/css/p3cbd.css"/>
      <script src="/js/cbd.js" type="text/javascript">
        <xsl:text> </xsl:text>
      </script>
<!-- Google tag (gtag.js) -->
<script async="async" src="https://www.googletagmanager.com/gtag/js?id=G-0QKC3P5HJ1"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());

  gtag('config', 'G-0QKC3P5HJ1');
</script>      
    </head>
    <body>
      <xsl:apply-templates/>
    </body>
  </html>
</xsl:template>

<xsl:template match="xff:sigs">
  <xsl:variable name="forms" select="."/>
  <h2 class="xff">ePSD2 Forms: <xsl:value-of select="@formset"/> (<xsl:value-of select="@eid"/>)</h2>
  <div class="xff">
  <table class="xff">
    <tr>
      <xsl:for-each select="document('')/*/xsl:template[@name='columns']/*">
        <th class="xff">
	  <xsl:if test="$forms/@sortopt=@attr">
	    <xsl:attribute name="class">xffsortopt</xsl:attribute>
	  </xsl:if>
	  <xsl:choose>
  	    <xsl:when test="@sortable='yes'">
	      <xsl:if test="@reversible='yes'">
 	        <xsl:variable name="arrow">
	          <xsl:choose>
		    <xsl:when test="$forms/@arrow='down'">up</xsl:when>
		    <xsl:otherwise>down</xsl:otherwise>
       	          </xsl:choose>
	        </xsl:variable>
 	        <a href="/{$project}/xff/{$forms/@eid}?sortopt={@attr}&amp;arrow={$arrow}" class="xff">
	 	  <img class="xff" src="/img/{$forms/@arrow}.gif"/>
 	        </a>
	      </xsl:if>
	      <xsl:text>&#xa0;</xsl:text>
  	      <a href="/{$project}/xff/{$forms/@eid}?sortopt={@attr}" class="xff">
		<xsl:value-of select="@th"/>
	      </a>
	    </xsl:when>
	    <xsl:otherwise>
  	      <xsl:value-of select="@th"/>
	    </xsl:otherwise>
	  </xsl:choose>
 	</th>
      </xsl:for-each>
    </tr>
    <xsl:apply-templates/>
  </table>
  </div>
</xsl:template>

<xsl:template match="xff:sig">
  <tr>
    <xsl:if test="position() mod 2 = 0">
      <xsl:attribute name="class">even</xsl:attribute>
    </xsl:if>
<!--
    <xsl:call-template name="td">
      <xsl:with-param name="at" select="@cfgw"/></xsl:call-template>
    <xsl:call-template name="td">
      <xsl:with-param name="at" select="@pos"/></xsl:call-template>
 -->
    <xsl:call-template name="tda">
      <xsl:with-param name="at" select="xff:form/@n"/></xsl:call-template>
    <xsl:call-template name="tda-node">
      <xsl:with-param name="at" select="xff:base"/></xsl:call-template>
    <xsl:call-template name="tda-node">
      <xsl:with-param name="at" select="xff:morph"/></xsl:call-template>
    <xsl:call-template name="tda">
      <xsl:with-param name="at" select="@icount"/></xsl:call-template>
  </tr>
</xsl:template>

<xsl:template name="tda">
  <xsl:param name="at"/>
  <td class="xff">
    <a href="javascript:distprof2('{$project}','{/*/@xml:lang}','{@xis}')">
      <xsl:call-template name="html-text">
        <xsl:with-param name="text" select="$at"/>
      </xsl:call-template>
    </a>
  </td>
</xsl:template>

<xsl:template name="tda-node">
  <xsl:param name="at"/>
  <td class="xff">
    <a href="javascript:distprof2('{$project}','{/*/@xml:lang}','{$at/@xis}')">
      <xsl:call-template name="html-text">
        <xsl:with-param name="text" select="$at/@n"/>
      </xsl:call-template>
    </a>
  </td>
</xsl:template>

<xsl:template name="td">
  <xsl:param name="at"/>
  <td class="xff">
    <xsl:call-template name="html-text">
      <xsl:with-param name="text" select="$at"/>
    </xsl:call-template>
  </td>
</xsl:template>

</xsl:stylesheet>
