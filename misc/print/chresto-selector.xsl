<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
		xmlns="http://oracc.org/ns/cbd/1.0"
		xmlns:cbd="http://oracc.org/ns/cbd/1.0"
		xmlns:xis="http://oracc.org/ns/xis/1.0"
		xmlns:g="http://oracc.org/ns/gdl/1.0"
		xmlns:xi="http://www.w3.org/2001/XInclude"
		exclude-result-prefixes="xi"
		>
<xsl:include href="xpd.xsl"/>
<xsl:include href="xtr-label.xsl"/>

<xsl:param name="cf-type" select="'writing'"/>
<xsl:param name="cbd-lang" select="'sux'"/>
<xsl:param name="with-sense" select="'yes'"/>
<xsl:param name="inline-refs" select="'yes'"/>

<xsl:template match="/">
  <xsl:apply-templates mode="pull" select="*/cbd:entries[@xml:lang=$cbd-lang]"/>
</xsl:template>

<xsl:template mode="pull" match="cbd:entries">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="pull"/>
  </xsl:copy>
</xsl:template>

<xsl:template mode="pull" match="cbd:letter">
  <xsl:apply-templates mode="pull" select="cbd:entry"/>
</xsl:template>

<xsl:template mode="pull" match="cbd:entry">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:choose>
      <xsl:when test="$cf-type='writing'">
	<xsl:choose>
	  <xsl:when test="cbd:compound">
	    <xsl:attribute name="compound">
	      <xsl:text>yes</xsl:text>
	    </xsl:attribute>
	    <xsl:variable name="lit">
	      <xsl:for-each select="cbd:compound/cbd:cpd">
		<xsl:value-of select="id(@eref)/cbd:bases/cbd:base[1]/@literal"/>
		<xsl:if test="not(position()=last())">
		  <xsl:choose>
		    <xsl:when test="position()+1=last()
		                    and contains(following-sibling::cbd:cpd[1]/cbd:pos,'V')">
		      <xsl:text>...</xsl:text>
		    </xsl:when>
		    <xsl:otherwise>
		      <xsl:text> </xsl:text>
		    </xsl:otherwise>
		  </xsl:choose>				  
		</xsl:if>
	      </xsl:for-each>
	    </xsl:variable>
	    <cf literal="{$lit}">
	      <xsl:for-each select="cbd:compound/cbd:cpd">
		<t ref="{id(@eref)/cbd:bases/cbd:base[1]/cbd:t[1]/g:w[1]/@xml:id}"/>
	      </xsl:for-each>
	    </cf>
	  </xsl:when>
	  <xsl:otherwise>
	    <cf literal="{cbd:bases/cbd:base[1]/@literal}">
	      <xsl:copy-of select="cbd:bases/cbd:base[1]/cbd:t[1]"/>
	    </cf>
	    <!-- FIXME: generate cross-references from other writings -->
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:when>
      <xsl:otherwise>
	<xsl:copy-of select="*[not(self::cbd:senses)]"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:if test="$with-sense='yes'">
      <xsl:copy-of select="cbd:senses"/>
    </xsl:if>
    <xsl:if test="$inline-refs='yes'">
      <xsl:variable name="xis-id" select="substring-after(concat(ancestor::cbd:entries/@n,'_',@xml:id), 'glossary_')"/>
<!--      <xsl:message>xis-id = <xsl:value-of select="$xis-id"/></xsl:message> -->
      <xsl:for-each select="id($xis-id)">
	<xsl:for-each select="id(@xis)/*">
	  <xsl:variable name="ref" select="substring-after(text(),':')"/>
	  <xsl:variable name="text" select="substring-before($ref,'.')"/>
	  <r ref="{text()}" tref="{$text}">
	    <xsl:attribute name="lref">
	      <xsl:value-of select="substring-before(substring-after($ref,'.'),'.')"/>
	    </xsl:attribute>
	    <xsl:attribute name="wref">
	      <xsl:value-of select="substring-after(substring-after($ref,'.'),'.')"/>
	    </xsl:attribute>
	    <xsl:variable name="name-node" 
			  select="document('names.xml',/)/*/group[@name='index']/n[@text=$text]"/>
	    <text sort="{$name-node/@sort}">
	      <xsl:value-of select="$name-node/text()"/>
	    </text>
	    <line>
	      <xsl:call-template name="xtr-format-label">
		<xsl:with-param name="format" select="id($ref)/../@label"/>
	      </xsl:call-template>
	    </line>
	  </r>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:if>
  </xsl:copy>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
