<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xst">

<xsl:output method="xml" encoding="utf-8" indent="yes" omit-xml-declaration="yes"/>

<xsl:param name="machine" select="'enlil'"/>

<xsl:variable name="letters" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'"/>
<xsl:variable name="spaces"  select="'                                    '"/>

<xsl:template match="xst:units">
  <html xmlns="http://www.w3.org/1999/xhtml" lang="sux" xml:lang="sux">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=UTF-8"/>
    <title>PPCS: <xsl:value-of select="@n"/></title>
    <xsl:choose>
      <xsl:when test="$machine = 'enlil'">
    <link rel="stylesheet" type="text/css" href="/oracc/www/ppcs/project.css"/>
    <script src="/oracc/www/oracc.js" type="text/javascript">
      <xsl:text> </xsl:text>
    </script>
    <script src="/oracc/www/ppcs/project.js" type="text/javascript">
      <xsl:text> </xsl:text>
    </script>
      </xsl:when>
      <xsl:otherwise>
    <link rel="stylesheet" type="text/css" href="../../ppcs.css"/>
    <script src="../../cbd.js" type="text/javascript">
      <xsl:text> </xsl:text>
    </script>
      </xsl:otherwise>
    </xsl:choose>
  </head>
  <body>
    <!-- HEADER PART -->
<!--
    <div class="banner">
    <table class="banner">
      <tr>
        <td class="logo">
	  <a class="logo" href="http://psd.museum.upenn.edu/ppcs">ppcs</a>
        </td>
        <td class="fullbutton">
	  <a class="fb" 
	     href="javascript:treatment('oracctreatment','/cgi-bin/oraccget?project=noracc&amp;item={@xml:id}')"
	     ><xsl:text>ORACC</xsl:text></a></td>
        <td class="fullbutton">
	  <a class="fb"
	     href="javascript:treatment('epsdtreatment','/cgi-bin/oraccget?project=epsd&amp;item={@xml:id}')"
	     ><xsl:text>ePSD</xsl:text></a></td>
	<td class="fullbutton">
	  <xsl:variable name="popup-id">"<xsl:value-of select="@xml:id"/>"</xsl:variable>
	  <a class="fb" href="{concat('javascript:catinfo(',$popup-id,')')}"
	     >Catalog</a>
	</td>
	<td class="fullbutton">
	  <a class="fb" href="{'javascript:epsdcopyright()'}"><xsl:text>Copyright</xsl:text></a>
	</td>
      </tr>
    </table>
    </div>
 -->
    <div class="references">
      <xsl:apply-templates/>
    </div>
    <div class="trailer">
<p class="qn">
<a href="http://validator.w3.org/check?uri=referer"><img
 src="http://www.w3.org/Icons/valid-xhtml10" alt="Valid XHTML 1.0!" 
height="21" width="60" /></a>
<a href="http://jigsaw.w3.org/css-validator/check/referer">
  <img style="border:0;width:60px;height:21px"
       src="http://jigsaw.w3.org/css-validator/images/vcss" 
       alt="Valid CSS!" />
 </a>
PPCS contacts: <a href="mailto:fumik@sas.upenn.edu">Fumi Karahashi</a> and
<a href="mailto:stinney@sas.upenn.edu">Steve Tinney</a>.</p>
  </div>
  </body>
  </html>
</xsl:template>

<xsl:template match="xst:comment">
  <p class="comment">(<xsl:value-of select="text()"/>)</p>
  <hr/>
</xsl:template>

<xsl:template match="xst:unit">
  <xsl:param name="clean" select="false()"/>
  <xsl:if test="not($clean)">
    <h1 class="reflabel">
      <a name="{substring-after(@ref,'.')}">
	<span class="refid">[<xsl:value-of select="@ref"/>]</span>
	<xsl:text> </xsl:text>
	<xsl:value-of select="@n"/>
      </a>
    </h1>
  </xsl:if>

  <xsl:if test="not($clean)">
    <hr class="buttonsep"/>
  </xsl:if>
  <pre>
    <xsl:apply-templates mode="xst"/>
    <xsl:text>&#xa;</xsl:text>
  </pre>
  <xsl:if test="not($clean)">
    <hr class="unitsep"/>
  </xsl:if>
</xsl:template>

<xsl:template mode="xst" match="xst:struct[position()=1 
				           or xst:feat[@type='ms-case']
	                                   or not(xst:feat[@type='modified']
        	           	                  and not(xst:feat[@type='parenthetic']))]">
  <xsl:choose>
    <xsl:when test="xst:feat[@type='syn-cat']='S'
		    and count(xst:struct/xst:struct) = 1">
      <xsl:apply-templates mode="xst" select="xst:struct"/>
    </xsl:when>
    <xsl:when test="xst:feat[@type='syn-cat']">
      <xsl:choose>
     	<xsl:when test="not(preceding-sibling::xst:struct) 
		    and preceding-sibling::xst:feat[@type='stype']">
          <xsl:text>&#xa;</xsl:text>
          <xsl:call-template name="indent"/>
	</xsl:when>
<!--
        <xsl:when test="preceding-sibling::xst:struct
			and count(xst:struct|descendant::xst:struct) > 1">
 -->
	<xsl:when test="preceding-sibling::xst:struct">
          <xsl:text>&#xa;</xsl:text>
          <xsl:call-template name="indent"/>
        </xsl:when>
      </xsl:choose>
      <xsl:variable name="class">
 	<xsl:choose>
	  <xsl:when test="ancestor::xst:struct/xst:feat[text() = 'RRC']">
	    <xsl:text>inherit</xsl:text>
	  </xsl:when>
	  <xsl:when test="xst:feat[@type='ms-case']">
	    <xsl:value-of select="xst:feat[@type='ms-case']/text()"/>
    	  </xsl:when>
 	  <xsl:otherwise>
	    <xsl:value-of select="xst:feat[@type='syn-cat']/text()"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:variable>
      <span class="{concat('MC', $class, 
			   count(ancestor::xst:struct/xst:feat[@type='syn-cat'][text()='S']))}">
        <xsl:call-template name="opentag">
	  <xsl:with-param name="node" select="xst:feat[@type='syn-cat']"/>
	  <xsl:with-param name="sub"  
		select="xst:feat[not(@type='syn-cat')
				   and not(@type='sem-role')
				   and (not(text()='MOD'))
				   and not(text()='POS')]"/>
        </xsl:call-template>
	<xsl:if test="xst:feat[@type='ms-case'] and xst:feat[@type='parenthetic']">
          <xsl:text>&#xa;</xsl:text>
          <xsl:call-template name="indent">
	    <xsl:with-param name="syn-cat-spacers"
		select="ancestor-or-self::*[not(preceding-sibling::xst:struct)
				    	    or not(xst:feat[@type='modified'])]
					/xst:feat[@type='syn-cat']"/>
	    <xsl:with-param name="sub-spacers"
	  	select="ancestor-or-self::*/xst:feat[@type='ms-case']
			 |ancestor-or-self::*/xst:feat[@type='genitive']
			 |ancestor-or-self::*/xst:feat[@type='parenthetic']
			 |ancestor-or-self::*/xst:feat[@type='finiteness']
			"/>

<!--			 |ancestor-or-self::*/xst:feat[@type='possessive'] -->

	  </xsl:call-template>
	</xsl:if>
        <xsl:apply-templates mode="xst"/>
        <xsl:text>)</xsl:text>
      </span>
    </xsl:when>
    <xsl:when test="xst:feat[@type='ms-cat']">
      <xsl:choose>
     	<xsl:when test="not(preceding-sibling::xst:struct) 
		    and preceding-sibling::xst:feat[@type='stype']">
          <xsl:text>&#xa;</xsl:text>
          <xsl:call-template name="indent"/>
	</xsl:when>
	<xsl:when test="preceding-sibling::xst:struct/xst:feat[@type='syn-cat']
  	  	    and count(preceding-sibling::xst:struct) > 1
		    or  count(preceding-sibling::xst:struct[1]/xst:struct) > 1">
          <xsl:text>&#xa;</xsl:text>
          <xsl:call-template name="indent"/>
        </xsl:when>
	<xsl:when test="parent::xst:struct/xst:feat[text()='RRC']
			or parent::xst:struct/xst:feat[text()='LFD']">
          <xsl:text>&#xa;</xsl:text>
          <xsl:call-template name="indent"/>
	</xsl:when>
      </xsl:choose>

      <xsl:choose>
	<xsl:when test="xst:feat[@type='ms-cat']/text() = '0' and xst:data = '*'">
	  <xsl:text>*</xsl:text>
	</xsl:when>
	<xsl:otherwise>
      <xsl:call-template name="opentag">
        <xsl:with-param name="node" select="xst:feat[@type='ms-cat']"/>
        <xsl:with-param name="sub"  select="xst:feat[not(@type='ms-cat')]"/>
      </xsl:call-template>
      <xsl:for-each select="xst:data">
 	<span class="data">
  	  <xsl:value-of select="translate(xst:item,'ŋŊ','ĝĜ')"/>
	  <xsl:if test="xst:gloss">
	    <xsl:text>[</xsl:text>
	      <xsl:value-of select="xst:gloss/text()"/>
  	    <xsl:text>]</xsl:text>
	  </xsl:if>
	  <xsl:if test="xst:item/@rr">
	    <xsl:value-of select="concat('#',xst:item/@rr)"/>
	  </xsl:if>
        </span>
      </xsl:for-each>
      <xsl:text>)</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template mode="xst" name="opentag">
  <xsl:param name="node"/>
  <xsl:param name="sub"/>
  <xsl:text>(</xsl:text>
  <xsl:value-of select="$node/text()"/>
  <xsl:if test="$sub">
    <sub>
      <xsl:for-each select="$sub">
	<xsl:choose>
	  <xsl:when test="@type='resumptive'">
	    <xsl:text>RSP=</xsl:text>
	    <xsl:value-of select="text()"/>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:value-of select="text()"/>
	    <xsl:if test="position() &lt; last()">
	      <xsl:text>-</xsl:text>
            </xsl:if>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:for-each>
<!--      <xsl:value-of select="$sub"/> -->
      <xsl:choose>
        <xsl:when test="$sub = 'LT'">
	  <xsl:text> </xsl:text>
        </xsl:when>
        <xsl:when test="$sub = 'F'">
	  <xsl:text>  </xsl:text>
        </xsl:when>
      </xsl:choose>
<!--
      <xsl:choose>
        <xsl:when test="string-length($sub) = 2">
          <xsl:text> </xsl:text>
        </xsl:when>
        <xsl:when test="string-length($sub) = 1">
          <xsl:text>  </xsl:text>
        </xsl:when>
        <xsl:when test="string-length($sub) = 0">
          <xsl:text>   </xsl:text>
        </xsl:when>
      </xsl:choose>
 -->
    </sub>
  </xsl:if>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template mode="xst" name="indent">
  <xsl:param name="syn-cat-spacers"
	select="ancestor::*[not(preceding-sibling::xst:struct)
			    or xst:feat[@type='parenthetic']
			    or xst:feat[@type='ms-case']
			    or not(xst:feat[@type='modified'])]
			  /xst:feat[@type='syn-cat']"/>
  <xsl:param name="sub-spacers"
	select="ancestor::*/xst:feat[@type='ms-case']
		|ancestor::*/xst:feat[@type='genitive']
		|ancestor::*/xst:feat[@type='parenthetic']
		|ancestor::*/xst:feat[@type='finiteness']
			"/>

<!--			 |ancestor::*/xst:feat[@type='possessive'] -->

  <xsl:variable name="regspaces">
    <xsl:for-each select="$syn-cat-spacers">
      <xsl:value-of select="text()"/>
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="subspaces">
    <xsl:for-each select="$sub-spacers">
      <xsl:choose>
	<xsl:when test="@type='parenthetic'">
	  <xsl:if test="not(../xst:feat[@type='ms-case'])">
	    <xsl:value-of select="text()"/>
	  </xsl:if>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="text()"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:variable>
  <xsl:for-each select="$syn-cat-spacers">
<!--  select="ancestor::*/xst:feat[@type='syn-cat']"> -->
    <xsl:text>  </xsl:text>
  </xsl:for-each>
  <xsl:if test="string-length($regspaces) > 0">
    <xsl:value-of select="translate($regspaces,$letters,$spaces)"/>
  </xsl:if>
  <xsl:if test="string-length($subspaces) > 0">
<!--    <xsl:message>processing subspaces</xsl:message> -->
    <sub>
      <xsl:value-of select="translate($subspaces,$letters,$spaces)"/>
      <xsl:for-each select="ancestor::*/xst:feat[@type='ms-case'][text() = 'LT']">
	<xsl:message>adding LT space</xsl:message>
        <xsl:text> </xsl:text>
      </xsl:for-each>
    </sub>
  </xsl:if>
</xsl:template>

<xsl:template mode="xst" match="text()"/>

</xsl:stylesheet>
