<?xml version='1.0'?>

<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:xst="http://oracc.org/ns/syntax-tree/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  exclude-result-prefixes="xst">

<xsl:output method="text" encoding="utf-8"
   doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
   doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"
   indent="no"/>

<xsl:variable name="letters" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'"/>
<xsl:variable name="spaces"  select="'                                    '"/>

<xsl:template match="xst:units">
  <xsl:text>( (REF </xsl:text>
  <xsl:value-of select="concat('(CODE &lt;', translate(@n,' ','_'),'>)')"/>
  <xsl:text>)&#xa;</xsl:text>
  <xsl:value-of select="concat('  (ID ', @xml:id, ')')"/>
  <xsl:text>)&#xa;&#xa;</xsl:text>
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="xst:comment">
  <p class="comment">(<xsl:value-of select="text()"/>)</p>
  <hr/>
</xsl:template>

<xsl:template match="xst:unit">
<!--
  <h1 class="reflabel">
    <a name="{substring-after(@ref,'.')}">
    <span class="refid">[<xsl:value-of select="@ref"/>]</span>
    <xsl:text> </xsl:text>
    <xsl:value-of select="@n"/>
    </a>
  </h1>
  <hr class="buttonsep"/>
  <pre>
 -->
    <xsl:text>( </xsl:text>
    <xsl:apply-templates/>
    <xsl:text>&#xa;</xsl:text>
    <xsl:value-of select="concat('  (ID ', translate(@n,' ()','_&lt;>'), ')')"/>
    <xsl:text>)</xsl:text>
    <xsl:text>&#xa;&#xa;</xsl:text>
<!--
  </pre>
  <hr class="unitsep"/>
 -->
</xsl:template>

<xsl:template match="xst:struct[position()=1 
	or not(xst:feat[@type='modified']
		and not(xst:feat[@type='parenthetic']))]">
  <xsl:variable name="seg" select="preceding::xst:seg[1]"/>
  <xsl:choose>
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
<!--      <span class="{concat('MC', $class, 
			   count(ancestor::xst:struct/xst:feat[@type='syn-cat'][text()='S']))}">-->
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
        <xsl:apply-templates/>
        <xsl:text>)</xsl:text>
<!--      </span> -->
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
	  <xsl:variable name="struct" select="."/>
	  <xsl:for-each select="xst:data">
	    <!--<span class="data">-->
	    <!-- <xsl:value-of select="translate(xst:item,'ŋŊ','ĝĜ')"/> -->
	    <!-- <xsl:text>&lt;LEMMA </xsl:text> -->
	    <xsl:choose>
	      <xsl:when test="not(xst:gloss)">
		<xsl:value-of select="concat('*', translate(xst:item,'()','&lt;>'))"/>
		<xsl:text>)</xsl:text>
	      </xsl:when>
	      <xsl:otherwise>
<!--
		<xsl:text>&lt;FORM </xsl:text>
		<xsl:for-each select="document(concat(/*/@xml:id,'.xtf'),/)">
		  <xsl:value-of select="id($seg/@target)/@form"/>
		</xsl:for-each>
		<xsl:text>> </xsl:text>
		<xsl:text>&lt;LEMMA </xsl:text>
 -->

		<xsl:value-of select="concat('(FORM ', translate(xst:form,'()','&lt;>'), ')')"/>
		<xsl:value-of select="concat(' (LEMMA *&lt;', translate(xst:item,' ()','_&lt;>'), '[', translate(xst:gloss,' ','_'), ']', '>)')"/>

<!-- FIXME: reimplement @rr handling
		<xsl:if test="xst:item/@rr">
		  <xsl:value-of select="concat('#',xst:item/@rr)"/>
		</xsl:if>
 -->

<!--
		<xsl:value-of select="xst:item"/>
		<xsl:text>[</xsl:text>
		<xsl:value-of select="xst:gloss/text()"/>
		<xsl:text>]</xsl:text>
		<xsl:if test="xst:item/@rr">
		  <xsl:value-of select="concat('#',xst:item/@rr)"/>
		</xsl:if>
 -->
<!--
		<xsl:text>></xsl:text>
 -->
		<xsl:text> (CODE &lt;</xsl:text> <!-- ID -->
		  <xsl:value-of select="$seg/@target"/>
	        <xsl:text>>)</xsl:text>
		<xsl:text>)</xsl:text>
		<xsl:if test="$struct/following-sibling::xst:struct[1][not(xst:struct)]">
		  <xsl:text>&#xa;</xsl:text>
		  <xsl:call-template name="indent"/>
		</xsl:if>
	      </xsl:otherwise>
	    </xsl:choose>
	    <!--</span>-->
	  </xsl:for-each>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
  </xsl:choose>
</xsl:template>

<xsl:template name="opentag">
  <xsl:param name="node"/>
  <xsl:param name="sub"/>
  <xsl:text>(</xsl:text>
  <xsl:value-of select="$node/text()"/>
  <xsl:if test="$sub">
<!--    <sub> -->
      <xsl:text>-</xsl:text>
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
<!--    </sub> -->
  </xsl:if>
  <xsl:text> </xsl:text>
</xsl:template>

<xsl:template name="indent">
  <xsl:param name="syn-cat-spacers"
	select="ancestor::*[not(preceding-sibling::xst:struct)
			    or xst:feat[@type='parenthetic']
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
      <xsl:if test="not(position()=1)">
	<xsl:text> </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="subspaces">
    <xsl:for-each select="$sub-spacers">
      <xsl:value-of select="text()"/>
    </xsl:for-each>
  </xsl:variable>

  <xsl:text>  </xsl:text>

  <xsl:for-each select="$syn-cat-spacers">
<!--  select="ancestor::*/xst:feat[@type='syn-cat']"> -->
    <xsl:text>  </xsl:text>
  </xsl:for-each>
  <xsl:if test="string-length($regspaces) > 0">
    <xsl:value-of select="translate($regspaces,$letters,$spaces)"/>
  </xsl:if>
  <xsl:if test="string-length($subspaces) > 0">
<!--    <xsl:message>processing subspaces</xsl:message> -->
<!--    <sub> -->
      <xsl:value-of select="translate($subspaces,$letters,$spaces)"/>
      <xsl:for-each select="ancestor::*/xst:feat[@type='ms-case'][text() = 'LT']">
	<!-- <xsl:message>adding LT space</xsl:message> -->
        <xsl:text> </xsl:text>
      </xsl:for-each>
<!--    </sub> -->
  </xsl:if>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
