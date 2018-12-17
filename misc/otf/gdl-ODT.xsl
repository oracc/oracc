<?xml version='1.0' encoding='utf-8'?>

<xsl:stylesheet version="1.0" 
		xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
		xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0"
		xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
		xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0"
		xmlns:xlink="http://www.w3.org/1999/xlink"
		xmlns:oracc="http://oracc.org/ns/oracc/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:xtf="http://oracc.org/ns/xtf/1.0"
		xmlns:xcl="http://oracc.org/ns/xcl/1.0"
		xmlns:xff="http://oracc.org/ns/xff/1.0"
		xmlns:norm="http://oracc.org/ns/norm/1.0"
		xmlns:note="http://oracc.org/ns/note/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:xl="http://www.w3.org/1999/xlink"
		exclude-result-prefixes="g norm xcl xff xtf xl">

<xsl:include href="gdl-render-g.xsl"/>

<xsl:param name="project"/>

<xsl:param name="render-accents">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'render-accents'"/>
  </xsl:call-template>
</xsl:param>

<xsl:param name="render-hyphenate-phondets">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'render-hyphenate-phondets'"/>
  </xsl:call-template>
</xsl:param>

<xsl:param name="render-inter-det-char">
  <xsl:call-template name="xpd-option">
    <xsl:with-param name="option" select="'render-inter-det-char'"/>
    <xsl:with-param name="default" select="''"/>
  </xsl:call-template>
</xsl:param>

<xsl:variable name="dhyph" select="'--'"/>

<xsl:key name="lnodes" match="xcl:l" use="@ref"/>
<xsl:key name="xnodes" match="xcl:link" use="substring(@xl:href,2)"/>

<xsl:template match="g:gdl">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:w|norm:w">
  <xsl:param name="allow-space" select="not(@headform)"/>
  <xsl:call-template name="gdl-w">
    <xsl:with-param name="allow-space" select="$allow-space"/>
  </xsl:call-template>
  <xsl:if test="@headform"><xsl:text>-</xsl:text></xsl:if>
</xsl:template>

<xsl:template match="g:swc">
  <xsl:text>-</xsl:text>
  <xsl:choose>
    <xsl:when test="@swc-final = '0'">
      <xsl:call-template name="gdl-w">
	<xsl:with-param name="allow-space" select="false()"/>
      </xsl:call-template>
      <xsl:text>-</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="gdl-w">
	<xsl:with-param name="allow-space" select="true()"/>
      </xsl:call-template>      
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:v|norm:s">
  <xsl:variable name="lang1" select="ancestor-or-self::*[@xml:lang][1]/@xml:lang"/>
  <xsl:variable name="lang">
    <xsl:choose>
      <xsl:when test="contains($lang1, '-')">
	<xsl:value-of select="substring-before($lang1,'-')"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="$lang1"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="g-d" select="ancestor::g:d[1]"/>
  <xsl:call-template name="render-o"/>
  <xsl:choose>
    <xsl:when test="ancestor::g:d">
<!--      <xsl:message>render-hyphenate-phondets=<xsl:value-of select="$render-hyphenate-phondets"/></xsl:message> -->
      <text:span text:style-name="sup">
	<xsl:choose>
	  <xsl:when test="ancestor::g:d[1][@g:role='phonetic']">
	    <xsl:if test="$g-d and $render-hyphenate-phondets = 'yes' and $g-d/@g:pos='post'">
	      <xsl:text>-</xsl:text>
	    </xsl:if>
	    <text:span text:style-name="{$lang}">
	      <xsl:call-template name="render-g"/>
	    </text:span>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:call-template name="render-g"/>
	  </xsl:otherwise>
	</xsl:choose>
	<text:span text:style-name="r">
	  <xsl:call-template name="render-flags"/>
	</text:span>
	<xsl:if test="$g-d and $render-hyphenate-phondets = 'yes' and $g-d/@g:pos=pre">
	  <xsl:text>-</xsl:text>
	</xsl:if>
      </text:span>
    </xsl:when>
    <xsl:otherwise>
      <text:span>
	<xsl:attribute name="text:style-name">
	  <xsl:choose>
	    <xsl:when test="@norm:num='yes'">
	      <xsl:value-of select="'r'"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$lang"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:attribute>
	<xsl:call-template name="render-g"/>
      </text:span>
      <xsl:call-template name="render-flags"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="render-c"/>
  <xsl:if test="@note:mark">
    <xsl:call-template name="process-notes"/>
  </xsl:if>
  <xsl:value-of select="@g:delim"/>
<!--  <xsl:call-template name="deep-g-delim"/> -->
</xsl:template>

<xsl:template match="g:s">
  <xsl:if test="ancestor::g:d/@g:pos = 'post'">.</xsl:if>
  <xsl:variable name="sac-style">
    <xsl:choose>
      <xsl:when test="@g:logolang='akk'">
	<xsl:text>sac-akk</xsl:text>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>sac-sign</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:call-template name="render-o"/>
  <xsl:choose>
    <xsl:when test="ancestor::g:d and ancestor::g:q">
      <text:span text:style-name="sup">
	<text:span text:style-name="{$sac-style}">
	  <xsl:call-template name="render-g"/>
	</text:span>
	<xsl:call-template name="render-flags"/>
	<xsl:value-of select="@g:delim"/>
      </text:span>
    </xsl:when>
    <xsl:otherwise>
      <text:span text:style-name="{$sac-style}">
	<xsl:call-template name="render-g"/>
      </text:span>
      <xsl:call-template name="render-flags"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="render-c"/>
  <xsl:if test="@note:mark">
    <xsl:call-template name="process-notes"/>
  </xsl:if>
  <xsl:choose>
    <xsl:when test="not(ancestor::g:q) and ancestor::g:d 
  		   and not(ancestor::g:d/@g:pos = 'post')">
      <xsl:text>.</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@g:delim"/>
      <!--<xsl:call-template name="deep-g-delim"/>-->
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:d">
  <xsl:call-template name="render-o"/>
  <xsl:for-each select="*">
    <xsl:apply-templates select="."/>
  </xsl:for-each>
  <xsl:call-template name="render-flags"/>
  <xsl:call-template name="render-c"/>
  <xsl:if test="@note:mark">
    <xsl:call-template name="process-notes"/>
  </xsl:if>
  <!--<xsl:call-template name="deep-g-delim"/>-->
  <xsl:value-of select="@g:delim"/>
</xsl:template>

<xsl:template match="g:r">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="g:x">
  <xsl:choose>
    <xsl:when test="@g:type = 'newline'">
      <xsl:choose>
	<xsl:when test="preceding-sibling::g:w">
	  <xsl:text>&#xa0;; </xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:text>;</xsl:text>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:when test="@g:type='dollar' or @g:type='comment'">
      <xsl:value-of select="@g:o"/>
      <xsl:value-of select="concat('(',text(),')')"/>
      <xsl:if test="@note:mark">
	<xsl:call-template name="process-notes"/>
      </xsl:if>
    <xsl:value-of select="@g:c"/>    </xsl:when>
    <xsl:otherwise>
      <text:span text:style-name="r"> <!-- text:classes="r" -->
	<xsl:value-of select="@g:o"/>
	<xsl:value-of select="text()"/>
	<xsl:if test="@note:mark">
	  <xsl:call-template name="process-notes"/>
	</xsl:if>
	<xsl:value-of select="@g:c"/>
      </text:span>
    </xsl:otherwise>
  </xsl:choose>
  <!--<xsl:call-template name="deep-g-delim"/>-->
  <xsl:value-of select="@g:delim"/>
</xsl:template>

<xsl:template match="g:b">
  <xsl:choose>
    <xsl:when test="g:o">
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:p">
  <xsl:call-template name="render-o"/>
  <xsl:value-of select="@g:type"/>
  <xsl:if test="*">
    <xsl:text>(</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>)</xsl:text>
  </xsl:if>
  <xsl:call-template name="render-flags"/>
  <xsl:call-template name="render-c"/>
  <xsl:if test="@note:mark">
    <xsl:call-template name="process-notes"/>
  </xsl:if>
  <!--<xsl:call-template name="deep-g-delim"/>-->
  <xsl:value-of select="@g:delim"/>
</xsl:template>

<xsl:template match="g:q">
  <xsl:call-template name="render-o"/>
  <xsl:apply-templates select="*[1]"/>
  <xsl:choose>
    <xsl:when test="ancestor::g:d">
      <text:span text:style-name="sup"><xsl:text>(</xsl:text></text:span>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:apply-templates select="*[2]"/>
  <xsl:choose>
    <xsl:when test="ancestor::g:d">
      <text:span text:style-name="sup"><xsl:text>)</xsl:text></text:span>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="render-flags"/>
  <xsl:call-template name="render-c"/>
  <!--<xsl:call-template name="deep-g-delim"/>-->
  <xsl:value-of select="@g:delim"/>
</xsl:template>

<xsl:template match="g:a">
  <sub><xsl:apply-templates/></sub>
</xsl:template>

<xsl:template match="g:m">
  <sub>
    <xsl:text>@</xsl:text>
    <xsl:apply-templates/>
  </sub>
</xsl:template>

<xsl:template match="g:n">
  <xsl:call-template name="render-o"/>
  <xsl:choose>
    <xsl:when test="ancestor::g:d">
      <text:span text:style-name="sup">
	<xsl:if test="ancestor::g:d[@g:role='phonetic']">
	  <xsl:attribute name="class">
	    <xsl:value-of select="ancestor-or-self::*[@xml:lang]/@xml:lang"/>
	  </xsl:attribute>
	</xsl:if>
	<xsl:call-template name="render-n"/>
	<xsl:apply-templates select="g:a|g:m"/>
	<xsl:call-template name="render-flags"/>
      </text:span>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="render-n"/>
      <xsl:apply-templates select="g:a|g:m"/>
      <xsl:call-template name="render-flags"/>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:call-template name="render-c"/>
  <xsl:if test="@note:mark">
    <xsl:call-template name="process-notes"/>
  </xsl:if>
  <!--<xsl:call-template name="deep-g-delim"/>-->
  <xsl:value-of select="@g:delim"/>
</xsl:template>

<xsl:template name="render-n">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:for-each select="g:b">
        <xsl:apply-templates select="g:r"/>
        <xsl:text>(</xsl:text>
        <xsl:apply-templates select="*[2]"/>
	<xsl:apply-templates select="g:a|g:m"/>
        <xsl:text>)</xsl:text>
      </xsl:for-each>
     </xsl:when>
    <xsl:otherwise>
      <xsl:apply-templates select="g:r"/>
      <xsl:if test="count(*[not(self::g:m) and not(self::g:a)])>1">
	<xsl:text>(</xsl:text>
	<xsl:apply-templates select="*[2]"/>
	<xsl:text>)</xsl:text>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:c">
  <xsl:call-template name="render-o"/>
  <text:span text:style-name="compound">
    <xsl:apply-templates/>
    <xsl:call-template name="render-flags"/>
  </text:span>
  <xsl:call-template name="render-c"/>
  <xsl:if test="@note:mark">
    <xsl:call-template name="process-notes"/>
  </xsl:if>
<!--  <xsl:call-template name="deep-g-delim"/> -->
</xsl:template>

<xsl:template match="g:g">
  <xsl:choose>
    <xsl:when test="g:b">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:o">
  <xsl:choose>
    <xsl:when test="@g:type='beside'"><xsl:text>.</xsl:text></xsl:when>
    <xsl:when test="@g:type='joining'"><xsl:text>+</xsl:text></xsl:when>
    <xsl:when test="@g:type='containing'"><xsl:text>×</xsl:text></xsl:when>
    <xsl:when test="@g:type='above'"><xsl:text>&amp;</xsl:text></xsl:when>
    <xsl:when test="@g:type='crossing'"><xsl:text>%</xsl:text></xsl:when>
    <xsl:when test="@g:type='opposing'"><xsl:text>@</xsl:text></xsl:when>
    <xsl:when test="@g:type='reordered'"><xsl:text>:</xsl:text></xsl:when>
    <xsl:when test="@g:type='repeated'">
      <xsl:value-of select="concat(.,'x')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>gdl-HTML: operator <xsl:value-of select="@g:type"/> not handled</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="g:gg">
  <xsl:choose>
    <xsl:when test="@g:type='correction'">
      <xsl:apply-templates select="*[1]"/>
      <xsl:text>(</xsl:text>
	<xsl:for-each select="*[position()>1]">
	  <xsl:apply-templates select="."/>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>-</xsl:text>
          </xsl:if>
 	</xsl:for-each>
      <xsl:text>)</xsl:text>
    </xsl:when>
    <xsl:when test="@g:type='alternation'">
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
	<xsl:if test="not(position()=last())">
	  <xsl:text>/</xsl:text>
        </xsl:if>
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="@g:type='ligature'">
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
<!--	<xsl:value-of select="@g:delim"/> -->
	<!--
	<xsl:if test="not(position()=last())">
          <xsl:text>+</xsl:text>
        </xsl:if>
	-->
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="@g:type='group' or @g:type='logo'">
      <xsl:variable name="boundary">
	<xsl:value-of select="@g:delim"/>
<!--
	<xsl:choose>
	  <xsl:when test="g:s|g:c|g:n">
	    <xsl:text>.</xsl:text>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:text>-</xsl:text>
	  </xsl:otherwise>
        </xsl:choose>
 -->
      </xsl:variable>
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
	<!--
	<xsl:choose>
	  <xsl:when test="self::g:d[@g:pos='pre']
			  or following-sibling::*[1][self::g:d[@g:pos='post']]"/>
	  <xsl:otherwise>
	    <xsl:if test="not(position()=last())">
	      <xsl:choose>
		<xsl:when test="@g:delim">
		  <xsl:value-of select="@g:delim"/>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:value-of select="$boundary"/>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:if>
	  </xsl:otherwise>
	  </xsl:choose>
	  -->
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="@g:type='reordering'">
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
	<xsl:if test="not(position()=last())">
	  <xsl:text>:</xsl:text>
        </xsl:if>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <xsl:message>gdl-ODT: group type <xsl:value-of select="@g:type"/> not handled</xsl:message>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:if test="@note:mark">
    <xsl:call-template name="process-notes"/>
  </xsl:if>
<!--  <xsl:call-template name="deep-g-delim"/> -->
</xsl:template>

<xsl:template match="g:surro">
  <xsl:apply-templates select="*[1]"/>
  <xsl:text>&lt;(</xsl:text>
  <xsl:apply-templates select="*[2]"/>
  <xsl:text>)></xsl:text>
</xsl:template>

<xsl:template match="g:nonw">
  <xsl:choose>
    <xsl:when test="@type='vari'">
      <text:span text:style-name="sup" text:classes="varmark"><xsl:value-of select="concat('(:',.,':)')"/></text:span>
    </xsl:when>
    <xsl:when test="contains(.,'_')">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:when test="../self::xtf:surro or ../self::g:gsurro">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:when test="@type='punct' or @type='excised'">
      <xsl:for-each select="*">
	<xsl:apply-templates select="."/>
	<xsl:if test="not(position()=last())">
	  <xsl:choose>
	    <xsl:when test="@g:delim">
	      <xsl:value-of select="@g:delim"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text>-</xsl:text>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:if>
      </xsl:for-each>
      <xsl:text> </xsl:text>
    </xsl:when>
    <xsl:when test="@type='notelink'">
      <xsl:call-template name="process-notes"/>
    </xsl:when>
    <xsl:when test="@type='comment'"/>
    <xsl:otherwise>
      <!-- it's wrong to include pre-spaces here; handle that with word
	   spacing
	-->
      <xsl:text>(</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>)</xsl:text>
      <xsl:if test="following-sibling::*">
	<xsl:text> </xsl:text>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:variable name="subdig" select="'₀₁₂₃₄₅₆₇₈₉₊ₓ'"/>
<xsl:variable name="regdig" select="'0123456789xx'"/>

<xsl:template name="render-g-text">
  <xsl:param name="t"/>
  <xsl:variable name="body" select="translate($t,$subdig,'')"/>
  <xsl:value-of select="$body"/>
  <xsl:if test="string-length($body) &lt; string-length($t)">
    <text:span text:style-name="r">
<!--
      <xsl:value-of select="translate(substring(.,1+string-length($body)),
	                              $subdig,$regdig)"/>
 -->
      <xsl:value-of select="substring($t,1+string-length($body))"/>
    </text:span>
  </xsl:if>
</xsl:template>

<xsl:template name="render-flags">
  <xsl:if test="@g:collated
		or @g:queried
		or @g:remarked">
    <text:span text:style-name="flags">
      <xsl:if test="@g:collated"><xsl:text>*</xsl:text></xsl:if>
      <xsl:if test="@g:remarked"><xsl:text>!</xsl:text></xsl:if>
      <xsl:if test="@g:queried"><xsl:text>?</xsl:text></xsl:if>
    </text:span>
  </xsl:if>
</xsl:template>

<xsl:template name="render-o">
  <xsl:if test="@g:varo">
    <text:span text:style-name="sup" text:classes="varmark"><xsl:value-of select="concat('(',@g:varo,':')"/></text:span>
  </xsl:if>
  <xsl:value-of select="@g:o"/>
  <xsl:if test="@g:ho">
    <xsl:text>&#x2e22;</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="render-c">
  <xsl:if test="@g:hc">
    <xsl:text>&#x2e23;</xsl:text>
  </xsl:if>
  <xsl:value-of select="@g:c"/>
  <xsl:if test="@g:varc">
    <text:span text:style-name="sup" text:classes="varmark"><xsl:value-of select="concat(':',@g:varc,')')"/></text:span>
  </xsl:if>
</xsl:template>

<xsl:template name="gdl-w">
  <xsl:param name="allow-space" select="true()"/>
  <xsl:variable name="lang1" select="ancestor::*[@xml:lang][1]/@xml:lang"/>

  <xsl:variable name="lnodes" select="key('lnodes',@xml:id)"/>
  <xsl:variable name="xnode" select="key('xnodes',$lnodes[1]/@xml:id)"/>

  <xsl:variable name="form-id">
    <xsl:choose>
      <xsl:when test="$xnode"><xsl:value-of select="$xnode/../@xml:id"/></xsl:when>
      <xsl:otherwise><xsl:value-of select="$lnodes[1]/@xml:id"/></xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <xsl:variable name="lemform" select="id($form-id)/xff:f"/>
  <xsl:variable name="lemdict" select="$lemform/following-sibling::*[1]"/>

  <xsl:variable name="ltitle">
    <xsl:if test="$lemdict">
      <xsl:value-of select="concat($lemform/@norm0,
			           '[',$lemform/@sense,']',
				   $lemform/@epos)"/>
    </xsl:if>
  </xsl:variable>
    <text:span text:style-name="w">
      <xsl:choose>
	<xsl:when test="self::norm:w">
	  <!--FIXME: norm:w should support lemmatization also-->
	  <xsl:call-template name="render-o"/>
	  <xsl:choose>
	    <xsl:when test="not(@xml:lang='sux')">
	      <text:span>
		<xsl:attribute name="text:style-name">
		  <xsl:value-of select="ancestor-or-self::*[@xml:lang]/@xml:lang"/>
		</xsl:attribute>
		<xsl:apply-templates/>
	      </text:span>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:apply-templates/>
	    </xsl:otherwise>
	  </xsl:choose>
	  <xsl:call-template name="render-flags"/>
	  <xsl:call-template name="render-c"/>
	</xsl:when>
	<xsl:when test="string-length($ltitle) > 0">
	  <xsl:attribute name="title">
	    <xsl:value-of select="$ltitle"/>
	  </xsl:attribute>
	  <xsl:choose>
	    <xsl:when test="$lemdict/xcl:r[@type='entry']">
	      <text:a text:classes="cbd" 
		      office:target-frame-name="cbdentry"
		      office:title="{$project}/{$lemdict/@ref}/{$lemdict/xcl:r[@type='entry']}"
		      xlink:show="replace"
		      xlink:href="http://cdl.museum.upenn.edu/{$project}/{$lemdict/@ref}/{$lemdict/xcl:r[@type='entry']/@ref}.html">
		<xsl:call-template name="render-word"/>
	      </text:a>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:call-template name="render-word"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:call-template name="render-word"/>
	</xsl:otherwise>
      </xsl:choose>
    </text:span>
  <xsl:if test="$allow-space and following-sibling::*">
    <xsl:text> </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="render-word">
  <xsl:for-each select="*">
    <xsl:apply-templates select="."/>
    <xsl:choose>
      <xsl:when test="self::g:d and following-sibling::*[1][self::g:d]">
	<xsl:value-of select="$render-inter-det-char"/>
      </xsl:when>
      <xsl:otherwise>
	<!--<xsl:call-template name="maybe-hyphen"/>-->
	<!--
	<xsl:if test="not(@g:delim=$dyph)"> 
	  <xsl:value-of select="@g:delim"/>
	  </xsl:if>
	-->
	</xsl:otherwise>	  
    </xsl:choose>
  </xsl:for-each>
</xsl:template>

<xsl:template name="maybe-hyphen">
  <xsl:variable name="hyph" select="@g:delim"/>
  <xsl:choose>
    <xsl:when test="self::g:d">
      <xsl:if test="@g:pos='post' and not(position()=last())">
	<xsl:value-of select="$hyph"/>
      </xsl:if>
    </xsl:when>
    <xsl:when test="following-sibling::*[1][self::g:d]">
      <xsl:if test="following-sibling::*[1][@g:pos='pre']">
	<xsl:value-of select="$hyph"/>
      </xsl:if>
    </xsl:when>
    <xsl:when test="@gloss='cont' 
		    or (@gloss='post' and following::xtf:g[1]/@gloss='cont')">
      <!-- no-op -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:if test="not(position() = last())
		and not(@gloss = 'pre')
	        and not(following::xtf:g[1]/@gloss='post')
	        and not(following::*[1][local-name()='gloss'])">
	<xsl:value-of select="$hyph"/>
      </xsl:if>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<!--FIXME: noteref can have more than one IDREF in it-->
<xsl:template name="process-notes">
  <!--
  <text:span text:style-name="sup">
    <xsl:value-of select="."/>
    </text:span>
  -->
  <xsl:apply-templates mode="print" select="id(@note:ref)"/>
</xsl:template>

<!--
<xsl:template name="deep-g-delim">
  <xsl:value-of select="@g:delim"/>
</xsl:template>  

<xsl:template name="real-deep-g-delim">
  <xsl:if test="not(parent::g:w)">
    <xsl:value-of select="@g:delim"/>
  </xsl:if>
</xsl:template>
-->

<xsl:template match="g:*">
  <xsl:message>gdl-ODT: <xsl:value-of select="name()"/> not handled</xsl:message>
  <xsl:apply-templates/>
</xsl:template>

</xsl:stylesheet>
