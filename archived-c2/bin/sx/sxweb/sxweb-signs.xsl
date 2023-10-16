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
<xsl:include href="sxweb-util.xsl"/>

<xsl:output method="xml" indent="yes" encoding="utf-8"/>

<xsl:param name="project"/>
<xsl:param name="hproject" select="translate($project,'/','-')"/>
<xsl:param name="with-stats"/>

<xsl:param name="snippets" select="'/Users/stinney/orc/emss/00etc/snippets.xml'"/>
<xsl:param name="snippetdir" select="'/emss/snippets'"/>

<xsl:template match="sl:signlist">
  <xsl:message>sxweb-signs.xsl processing <xsl:value-of select="count(sl:letter/sl:sign)"/> signs</xsl:message>
  <xsl:apply-templates select="sl:letter/sl:sign"/>
</xsl:template>

<xsl:template match="sl:sign">
<!--  <xsl:if test="$with-stats='yes'"><xsl:message>with-stats=yes</xsl:message></xsl:if> -->
  <ex:document href="{concat('signlist/00web/',@xml:id,'.xml')}"
    method="xml" encoding="utf-8"
    indent="yes">
    <xsl:variable name="nn" select="translate(@n,'|','')"/>
    <esp:page>
      <esp:name><xsl:value-of select="$nn"/></esp:name>
      <esp:title>
	<xsl:value-of select="$nn"/>
	<xsl:if test="/*/@project='pcsl'">
	  <xsl:if test="not(sl:smap) and not(sl:uage='0')">
	    <xsl:text>: </xsl:text>
	    <xsl:value-of select=".//sl:uname[1]"/>
	    <span class="pcsl-oid"><xsl:value-of select="concat(' [',@xml:id,']')"/></span>
	  </xsl:if>
	</xsl:if>
      </esp:title>
      <esp:url><xsl:value-of select="@xml:id"/></esp:url>
      <html>
	<head/>
	<body>
	  <xsl:call-template name="navbar"/>
	  <xsl:call-template name="sign-or-form"/>
	</body>
      </html>
    </esp:page>
  </ex:document>
</xsl:template>

<xsl:template name="navbar">
  <xsl:variable name="prev" select="preceding::sl:sign[1]"/>
  <xsl:variable name="next" select="following::sl:sign[1]"/>
  <xsl:if test="$prev|$next">
    <div class="navbar">
      <xsl:choose>
	<xsl:when test="$prev and $next">
	  <p style="text-align-last: justify">
	    <xsl:text>«</xsl:text>
	    <esp:link page="{$prev/@xml:id}"><xsl:value-of select="$prev/@n"/></esp:link>
	    <xsl:text>«</xsl:text>
	    <xsl:text> </xsl:text>
	    <xsl:text>»</xsl:text>
	    <esp:link page="{$next/@xml:id}"><xsl:value-of select="$next/@n"/></esp:link>
	    <xsl:text>»</xsl:text>
	  </p>
	</xsl:when>
	<xsl:when test="$prev">
	  <p>
	    <xsl:text>«</xsl:text>
	    <esp:link page="{$prev/@xml:id}"><xsl:value-of select="$prev/@n"/></esp:link>
	    <xsl:text>«</xsl:text>
	  </p>
	</xsl:when>
	<xsl:otherwise>
	  <p style="text-align-last: right">
	    <xsl:text>»</xsl:text>
	    <esp:link page="{$next/@xml:id}"><xsl:value-of select="$next/@n"/></esp:link>
	    <xsl:text>»</xsl:text>
	  </p>
	</xsl:otherwise>
      </xsl:choose>
    </div>
  </xsl:if>
</xsl:template>

<xsl:template match="sl:form">
  <xsl:call-template name="sign-or-form"/>
</xsl:template>

<xsl:template name="icount">
  <xsl:if test="@icount">
    <xsl:variable name="s">
      <xsl:if test="not(@icount = '1')">
	<xsl:text>s</xsl:text>
      </xsl:if>
    </xsl:variable>
    <p><esp:link url="/{$project}/{@xml:id}.tis"><xsl:value-of select="@icount"/>
    instance<xsl:value-of select="$s"/>
    attested in project <xsl:value-of select="$project"/>.</esp:link></p>
  </xsl:if>
</xsl:template>

<xsl:template name="compute-iframe-height">
  <xsl:param name="c"/>
  <xsl:variable name="total" select="$c/c/r[1]/c[2]"/>
  <xsl:variable name="nsubh" select="count($c/c/r[position()>1]/c[2][not(text()='0')])"/>
  <xsl:variable name="ncite">
    <xsl:choose>
      <xsl:when test="$total > 30">
	<!-- we are in selective mode; up to 5 instances from each period are shown -->
	<xsl:variable name="V">
	  <xsl:choose>
	    <xsl:when test="$c/c/r[2]/c[2] > 4">
	      <xsl:text>5</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$c/c/r[2]/c[2]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="IV">
	  <xsl:choose>
	    <xsl:when test="$c/c/r[3]/c[2] > 4">
	      <xsl:text>5</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$c/c/r[3]/c[2]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="III">
	  <xsl:choose>
	    <xsl:when test="$c/c/r[4]/c[2] > 4">
	      <xsl:text>5</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$c/c/r[4]/c[2]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="I">
	  <xsl:choose>
	    <xsl:when test="$c/c/r[5]/c[2] > 4">
	      <xsl:text>5</xsl:text>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$c/c/r[5]/c[2]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:value-of select="$V+$IV+$III+$I"/>
      </xsl:when>
      <xsl:otherwise>
	<!-- we are in inclusive mode; all the instances are shown -->
	<xsl:value-of select="$c/c/r[2]/c[2]+$c/c/r[3]/c[2]+$c/c/r[4]/c[2]+$c/c/r[5]/c[2]"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="subtot" select="$nsubh + $ncite"/>
  <!--<xsl:message><xsl:value-of select="$c/@xml:id"/> subtot = <xsl:value-of select="$subtot"/></xsl:message>-->
  <xsl:choose>
    <xsl:when test="$total > 30">
      <xsl:value-of select="40*($subtot+1)"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="40*$subtot"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sign-or-form">
  <div class="{$project}-{local-name(.)}">
    <xsl:if test="local-name() = 'form'">
      <xsl:attribute name="id">
	<xsl:value-of select="concat('form',count(preceding-sibling::sl:form))"/>
      </xsl:attribute>
      <h2>
	<b>
	  <xsl:choose>
	    <xsl:when test="/*/@project='pcsl'">
	      <xsl:value-of select="@n"/>
	      <xsl:text>: </xsl:text>
	      <xsl:value-of select=".//sl:uname"/>
	      <span class="pcsl-oid"><xsl:value-of select="concat(' [',@xml:id,']')"/></span>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:apply-templates select="sl:name[1]"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</b>
      </h2>
    </xsl:if>
    <xsl:if test="count(sl:form) > 0">
      <div class="formjumps">
	<p>
	  <xsl:text>Jump to form: </xsl:text>
	  <xsl:for-each select="self::*[not(sl:uage='0')]|sl:form">
	    <esp:link bookmark="form{count(preceding-sibling::sl:form)}">
	      <xsl:value-of select="@n"/>
	      <xsl:if test="sl:images/sl:i[@loc]">
		<xsl:text> = </xsl:text>
		<xsl:variable name="base" select="'../../../pctc'"/>
		<xsl:for-each select="sl:images/sl:i[@loc][1]">
		  <xsl:variable name="ref" select="@ref"/>
		  <xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
		  <esp:image height="40px" file="{$base}/{$header/@path}/{@loc}"
			     description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
		</xsl:for-each>
	      </xsl:if>
	    </esp:link>
	    <xsl:if test="not(position()=last())"><xsl:text>; </xsl:text></xsl:if>
	  </xsl:for-each>
	</p>
      </div>
    </xsl:if>
    <xsl:call-template name="icount"/>
    <div class="{$project}-info">
      <p style="font-size: 150%">
	<xsl:for-each select="sl:name[1]//*">
	  <xsl:choose>
	    <xsl:when test="@g:utf8"><xsl:value-of select="@g:utf8"/></xsl:when>
	    <xsl:otherwise/>
	  </xsl:choose>
	</xsl:for-each>
      </p>
      <xsl:if test="sl:list">
	<p>
	  Lists:
	  <xsl:for-each select="sl:list">
	    <xsl:value-of select="@n"/>
	    <xsl:if test="not(position()=last())">
	      <xsl:text>; </xsl:text>
	    </xsl:if>
	  </xsl:for-each>
	</p>
      </xsl:if>
      <xsl:if test="sl:form and count(preceding-sibling::sl:form) = 0"><hr/></xsl:if>
      <xsl:choose>
	<xsl:when test="@compoundonly='yes'">
	  <xsl:variable name="s">
	    <xsl:if test="contains(@cpd-refs, ' ')">
	      <xsl:text>s</xsl:text>
	    </xsl:if>
	  </xsl:variable>
	  <p>Occurs in the following compound<xsl:value-of select="$s"/>:
	  <xsl:for-each select="id(@cpd-refs)">
	    <xsl:text> </xsl:text>
	    <esp:link page="{ancestor-or-self::sl:sign[1]/@xml:id}">
	      <xsl:apply-templates select=".//sl:name[1]"/>
	      <xsl:if test="sl:images/sl:i[@loc]">
		<xsl:text> = </xsl:text>
		<xsl:for-each select="sl:images/sl:i[@loc][1]">
		  <xsl:call-template name="esp-sign-image"/>
		</xsl:for-each>
	      </xsl:if>
	    </esp:link>
	  </xsl:for-each>
	  <xsl:text>.</xsl:text>
	  </p>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:if test="/*/@project = 'pcsl'">
	    <xsl:choose>
	      <xsl:when test="not(sl:uage='0')">
		<div class="image-insts">
		  <table width="95%">
		    <tr>
		      <td width="5%" valign="top" class="ii-signs">
			<xsl:choose>
			  <xsl:when test="sl:images/sl:i[@loc]">
			    <xsl:variable name="base" select="'../../../pctc'"/>
			    <xsl:for-each select="sl:images/sl:i[@loc]">
			      <hr/>
			      <p>
				<xsl:variable name="ref" select="@ref"/>
				<xsl:variable name="header" select="/*/sl:iheader[@xml:id=$ref]"/>
				<span class="im-label"><xsl:value-of select="$header/@label"/>:</span><br/><br/>
				<esp:image width="100%" file="{$base}/{$header/@path}/{@loc}"
					   description="{$header/@label} image of {ancestor::*[sl:name]/sl:name[1]}"/>
			      </p>
			      <!--<xsl:if test="not(position()=last())"><hr/></xsl:if>-->
			    </xsl:for-each>
			  </xsl:when>
			  <xsl:otherwise>
			    <xsl:text>&#xa0;</xsl:text>
			  </xsl:otherwise>
			</xsl:choose>
		      </td>
		      <td width="93%" class="ii-insts">
			<xsl:variable name="o" select="@xml:id"/>
			<xsl:for-each select="document('sl-corpus-counts.xml',/)">
			  <xsl:variable name="c" select="id($o)"/>

			  <!--
			      <xsl:message><xsl:value-of select="$o"/>:
			      node-count=<xsl:value-of select="count($c/*)"/>:
			      <xsl:value-of select="$c/c/r[1]/c[1]"/>=<xsl:value-of select="$c/c/r[1]/c[2]"/></xsl:message>
			  -->
			  
			  <xsl:choose>
			    <xsl:when test="count($c/*)=0 or $c/c/r[1]/c[2]='0'">
			      <p>(No attestations in corpus)</p>
			    </xsl:when>
			    <xsl:otherwise>
			      <xsl:variable name="height">
				<xsl:call-template name="compute-iframe-height">
				  <xsl:with-param name="c" select="$c"/>
				</xsl:call-template>
			      </xsl:variable>
			      <iframe width="100%" height="{$height}" src="/pctc/inst/{$o}.html"/>
			    </xsl:otherwise>
			  </xsl:choose>
			</xsl:for-each>
		      </td>
		    </tr>
		  </table>
		</div>
	      </xsl:when>
	      <xsl:otherwise>
		<p><xsl:text>(This entry is not marked for inclusion in Unicode [uage=0])</xsl:text></p>
	      </xsl:otherwise>
	    </xsl:choose>
	  </xsl:if>
	</xsl:otherwise>
      </xsl:choose>
      <xsl:variable name="oid" select="@xml:id"/>
      <xsl:if test="$project='emss'">
	<xsl:for-each select="document($snippets)">
	  <xsl:for-each select="id($oid)">	  
	    <p>See the <esp:link url="{$snippetdir}/{$oid}.html">EMSS scrapbook page for this sign</esp:link>.</p>
	  </xsl:for-each>
	</xsl:for-each>
      </xsl:if>
    </div>
    <xsl:if test="count(sl:v)>0">
      <div class="{$project}-values">
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

    <xsl:if test="not(/*/@project = 'pcsl')">
      <xsl:call-template name="lex-sign"/>
    </xsl:if>

    <xsl:apply-templates select="sl:note"/>
    <xsl:call-template name="unicode-info"/>
    <xsl:apply-templates mode="rest"/>    
    <xsl:if test="count(sl:glo)>0">
      <div id="glodata">
	<h2 class="{$project}-glo">Glossary Attestations</h2>
	<xsl:for-each select="sl:v">
	  <xsl:if test="sl:glo">
	    <h2 class="psl-e"><span class="psl-ahead"><xsl:value-of select="@n"/></span></h2>
	    <xsl:apply-templates select="sl:glo"/>
	  </xsl:if>
	</xsl:for-each>
      </div>
    </xsl:if>
    <xsl:if test="sl:form">
      <div class="{$project}-signforms">
	<xsl:if test="not(/*/@project = 'pcsl')">
	  <h2 class="sl-signforms">Variant sign-forms</h2>
	</xsl:if>
	<xsl:apply-templates select="sl:form"/>
      </div>
    </xsl:if>
  </div>
</xsl:template>

<xsl:template match="sl:glo">
  <xsl:for-each select="*">
    <h3 class="{$project}-glo">
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
  ><esp:link hide-print="yes" url="/{$project}/cbd/sux/{@ref}.html"><xsl:value-of select="@cfgw"/></esp:link
  ><xsl:if test="$with-stats='yes'"><xsl:value-of select="concat(' (',@icount,'× / ',@ipct,'%)')"/></xsl:if>
  </td
  ></tr>
</xsl:template>

<xsl:template mode="rest" match="sl:v|sl:sort|sl:uphase|sl:utf8|sl:uname|sl:list|sl:name|sl:pname|sl:inote|sl:form|sl:unote|sl:note|sl:qs|sl:inherited|sl:uage|sl:sys|sl:smap|sl:images"/>

<xsl:template match="sl:listdef|sl:sysdef|sl:images|sl:compoundonly|sl:iheader"/>

<xsl:template match="sl:note">
  <p class="{$project}-note"><xsl:apply-templates/></p>
</xsl:template>

<xsl:template match="sl:inote"/>

<xsl:template mode="rest" match="sl:lit">
  <p class="{$project}-lit"><xsl:apply-templates/></p>
</xsl:template>

<xsl:template mode="rest" match="*">
  <xsl:message>sxweb-signs.xsl: tag <xsl:value-of select="local-name(.)"/> not handled</xsl:message>
</xsl:template>

<xsl:template name="unicode-info">
  <xsl:if test="starts-with(sl:utf8/@ucode,'x12')">
    <p>Unicode <xsl:value-of select="sl:utf8/@ucode"/> = <xsl:value-of select="@uname"/></p>
  </xsl:if>
</xsl:template>

<!--<xsl:template match="text()"/>-->

</xsl:transform>
