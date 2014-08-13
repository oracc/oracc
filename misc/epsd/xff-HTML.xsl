<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns:xff="http://oracc.org/ns/xff/1.0"
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="html-standard.xsl"/>

<xsl:output method="xml" indent="yes"/>
<xsl:output method="xml" encoding="utf-8"
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
   indent="yes"/>

<xsl:param name="formset" select="/*/@formset"/>
<xsl:param name="edit"/>
<xsl:param name="edit-form" select="''"/>
<xsl:param name="project"/>

<xsl:variable name="cgi" select="'/cgi-bin/xff'"/>

<xsl:template name="columns">
  <col th="Form" attr="form" sortable="yes"/>
<!--  <col th="CFGW" attr="cfgw" sortable="yes"/> -->
<!--  <col th="POS"  attr="pos"  sortable="yes" reversible="yes"/> -->
  <col th="Base" attr="base" sortable="yes"/>
  <col th="Morph" attr="morph" sortable="yes"/>
  <col th="Freq" attr="freq" sortable="yes" reversible="yes"/>
</xsl:template>

<xsl:template match="/">
<!--  <xsl:message>with-edit = <xsl:value-of select="edit"/></xsl:message> -->
  <html>
    <head>
      <title>ePSD Forms: <xsl:value-of select="$formset"/></title>
      <link rel="stylesheet" type="text/css" href="/css/p3cbd.css"/>
      <script src="/js/cbd.js" type="text/javascript">
        <xsl:text> </xsl:text>
      </script>
    </head>
      <xsl:choose>
	<xsl:when test="string-length($edit-form) > 0">
    <body class="editform">
	  <xsl:call-template name="edit-form"/>
    </body>
	</xsl:when>
	<xsl:otherwise>
    <body>
          <xsl:apply-templates/>
    </body>
	</xsl:otherwise>
      </xsl:choose>
  </html>
</xsl:template>

<xsl:template match="xff:sigs">
  <xsl:variable name="forms" select="."/>
<!--  <h1 class="xff">ePSD Forms: <xsl:value-of select="@formset"/></h1> -->
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
		    <xsl:when test="$forms/@reverse='yes'">up</xsl:when>
		    <xsl:otherwise>down</xsl:otherwise>
       	          </xsl:choose>
	        </xsl:variable>
<!-- removed edit=$edit -->
 	        <a href="{$cgi}?project={$project}&amp;sortopt={@attr}&amp;arrow={$arrow}&amp;xff={$forms/@eid}" class="xff">
	 	  <img class="xff" src="/img/{$arrow}.gif"/>
 	        </a>
	      </xsl:if>
	      <xsl:text>&#xa0;</xsl:text>
  	      <a href="{$cgi}?project={$project}&amp;sortopt={@attr}&amp;xff={$forms/@eid}" class="xff">
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
      <xsl:with-param name="at" select="xff:form/@icount"/></xsl:call-template>
<!--
    <xsl:if test="$edit='yes'">
       <td class="edit"><a
	  href="javascript:xffedit('{$formset}','{@xml:id}')"><img
	  src="http://enlil.museum.upenn.edu/cdl/www/icons/edit.gif"/></a></td>
     </xsl:if>
 -->
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
    <a href="javascript:distprof2('{$project}','{/*/@eid}','{$at/@xis}')">
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

<xsl:template name="edit-form">
  <xsl:for-each select="id($edit-form)">
  <form method="post" action="{$cgi}" target="xffmainwin">
    <input type="hidden" name="xff"    value="{$formset}"/>
    <input type="hidden" name="update" value="{@xml:id}"/>
    <table>
      <tr><th>Lang</th><th>RWS</th><th>Form</th><th>CFGW</th><th>POS</th><th>Base</th><th>Morph</th></tr>
     <tr>
 	<td><xsl:value-of select="@xml:lang"/></td>
	<td><xsl:value-of select="@rws"/></td>
	<td><input name="uform" size="15" value="{@form}"/></td>
	<td><input name="ucfgw" size="15" value="{@cfgw}"/></td>
	<td></td>
	<td><input name="ubase" size="15" value="{@base}"/></td>
	<td><input name="umorph" size="15" value="{@morph}"/></td>
	<td class="norules"><input type="submit" value="Done"/></td>
     </tr>
    </table>
  </form>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
