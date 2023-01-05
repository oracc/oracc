<?xml version="1.0" encoding="utf-8"?>
<xsl:transform 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:g="http://oracc.org/ns/gdl/1.0"
    xmlns:ex="http://exslt.org/common"
    xmlns:dc="http://dublincore.org/documents/2003/06/02/dces/"
    xmlns:xh="http://www.w3.org/1999/xhtml"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:esp="http://oracc.org/ns/esp/1.0"
    exclude-result-prefixes="sl dc xh"
    extension-element-prefixes="ex"
    version="1.0">

<!--<xsl:include href="formdiv.xsl"/>-->

<xsl:include href="lex-sign-lookup.xsl"/>
<xsl:include href="g2-gdl-HTML.xsl"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:param name="project"/>
<xsl:param name="with-stats"/>

<xsl:param name="snippets" select="'/Users/stinney/orc/emss/00etc/snippets.xml'"/>
<xsl:param name="snippetdir" select="'/emss/snippets'"/>

<xsl:template match="sl:sign">
<!--  <xsl:if test="$with-stats='yes'"><xsl:message>with-stats=yes</xsl:message></xsl:if> -->
  <ex:document href="{concat('signlist/00web/',@xml:id,'.xml')}"
    method="xml" encoding="utf-8"
    indent="yes">
    <xsl:variable name="nn" select="translate(@n,'|','')"/>
    <esp:page>
      <esp:name><xsl:value-of select="$nn"/></esp:name>
      <esp:title><xsl:value-of select="$nn"/></esp:title>
      <esp:url><xsl:value-of select="@xml:id"/></esp:url>
      <html>
	<head/>
	<body>
<!--
	  <xsl:call-template name="form-div">
	    <xsl:with-param name="caller" select="'esp'"/>
	  </xsl:call-template>
 -->
	  <xsl:call-template name="sign-or-form"/>
	</body>
      </html>
    </esp:page>
  </ex:document>
</xsl:template>

<xsl:template match="sl:form">
  <xsl:call-template name="sign-or-form"/>
</xsl:template>

<xsl:template name="sign-or-form">
  <div iclass="ogsl-{local-name(.)}">
    <div class="ogsl-info">
      <p style="font-size: 150%">
	<xsl:for-each select="sl:name[1]//*">
	  <xsl:if test="@g:utf8"><xsl:value-of select="@g:utf8"/></xsl:if>
	</xsl:for-each>
      </p>
      <xsl:if test="sl:list">
	<p>
	  List numbers: 
	  <xsl:for-each select="sl:list">
	    <xsl:value-of select="@n"/>
	    <xsl:if test="not(position()=last())">
	      <xsl:text>; </xsl:text>
	    </xsl:if>
	  </xsl:for-each>
	</p>
      </xsl:if>
      <xsl:variable name="oid" select="@xml:id"/>
      <xsl:for-each select="document($snippets)">
	<xsl:for-each select="id($oid)">	  
	  <p>See the <esp:link url="{$snippetdir}/{$oid}.html">EMSS scrapbook page for this sign</esp:link>.</p>
	</xsl:for-each>
      </xsl:for-each>
    </div>
    <xsl:if test="count(sl:v)>0">
      <div class="ogsl-values">
	<p>
	  <span class="values-heading">Values: </span>
	  <xsl:for-each select="sl:v">
	    <xsl:choose>
	      <xsl:when test="@deprecated='yes'">
		<span class="v-drop"><xsl:value-of select="@n"/></span>
	      </xsl:when>
	      <xsl:when test="@uncertain='yes'">
		<span class="v-query"><xsl:value-of select="@n"/></span>
	      </xsl:when>
	      <xsl:otherwise>
		<span class="v-ok"><xsl:value-of select="@n"/></span>
	      </xsl:otherwise>
	    </xsl:choose>
	    <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	  </xsl:for-each>
	  <xsl:text>.</xsl:text>
	</p>
      </div>
    </xsl:if>

    <xsl:call-template name="lex-sign"/>

    <xsl:apply-templates select="sl:note"/>
    <xsl:call-template name="unicode-info"/>
    <xsl:apply-templates mode="rest"/>    
    <xsl:if test="count(sl:v)>0">
      <div id="glodata">
	<h2 class="ogsl-glo">Glossary Attestations</h2>
	<xsl:for-each select="sl:v">
	  <xsl:if test="sl:glo">
	    <h2 class="psl-e"><span class="psl-ahead"><xsl:value-of select="@n"/></span></h2>
	    <xsl:apply-templates select="sl:glo"/>
	  </xsl:if>
	</xsl:for-each>
      </div>
    </xsl:if>
    <xsl:if test="sl:form">
      <div class="ogsl-signforms">
	<xsl:apply-templates select="sl:form"/>
      </div>
    </xsl:if>
  </div>
</xsl:template>

<xsl:template match="sl:glo">
  <xsl:for-each select="*">
    <h3 class="ogsl-glo">
      <xsl:choose>
	<xsl:when test="@type='s'"><xsl:text>Independent</xsl:text></xsl:when>
	<xsl:when test="@type='i'"><xsl:text>Initial</xsl:text></xsl:when>
	<xsl:when test="@type='m'"><xsl:text>Medial</xsl:text></xsl:when>
	<xsl:when test="@type='f'"><xsl:text>Final</xsl:text></xsl:when>
	<xsl:otherwise><xsl:message>sl:glo with unknown @type <xsl:value-of select="@type"/></xsl:message></xsl:otherwise>
      </xsl:choose>
    </h3>
    <table class="psl">
      <xsl:apply-templates/>
    </table>
  </xsl:for-each>
</xsl:template>

<xsl:template match="sl:glo-inst">
  <tr><td class="psl-eleft"><xsl:apply-templates/></td
  ><td class="psl-eright"
  ><esp:link hide-print="yes" url="/epsd2/cbd/sux/{@ref}.html"><xsl:value-of select="@cfgw"/></esp:link
  ><xsl:if test="$with-stats='yes'"><xsl:value-of select="concat(' (',@icount,'× / ',@ipct,'%)')"/></xsl:if>
  </td
  ></tr>
</xsl:template>

<xsl:template mode="rest" match="sl:v|sl:sort|sl:uphase|sl:utf8|sl:uname|sl:list|sl:name|sl:pname|sl:inote|sl:form|sl:unote|sl:note|sl:qs"/>

<xsl:template match="sl:note">
  <p class="ogsl-note"><xsl:apply-templates/></p>
</xsl:template>

<xsl:template mode="rest" match="*">
  <xsl:message>tag <xsl:value-of select="local-name(.)"/> not handled</xsl:message>
</xsl:template>

<xsl:template name="unicode-info">
  <xsl:if test="starts-with(sl:utf8/@ucode,'x12')">
    <p>Unicode <xsl:value-of select="sl:utf8/@ucode"/> = <xsl:value-of select="@uname"/></p>
  </xsl:if>
</xsl:template>

<!--<xsl:template match="text()"/>-->

</xsl:transform>
