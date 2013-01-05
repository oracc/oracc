<xsl:stylesheet version="1.0" 
  xmlns:xh="http://www.w3.org/1999/xhtml"
  xmlns:xtr="http://oracc.org/ns/xtr/1.0"
  xmlns:xtf="http://oracc.org/ns/xtf/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:ex="http://exslt.org/common"
  extension-element-prefixes="ex"
  exclude-result-prefixes="xh xtr">

<xsl:template name="xtr-label">
  <xsl:param name="config-xml" select="''"/>
  <xsl:variable name="tstyle-frag">
    <xsl:call-template name="xpd-option-node">
      <xsl:with-param name="config-xml" select="$config-xml"/>
      <xsl:with-param name="option" select="'render-tlat-style'"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:variable name="tstyle" select="ex:node-set($tstyle-frag)/*"/>
  <xsl:choose>
    <xsl:when test="$tstyle">
      <!--<xsl:message>tstyle = <xsl:value-of select="$tstyle/@value"/></xsl:message>-->
      <xsl:value-of select="$tstyle/*[@name='opener']/@value"/>
      <xsl:call-template name="xtr-format-label">
	<xsl:with-param name="config-xml" select="$config-xml"/>
	<xsl:with-param name="format" select="@xtr:lab-start-label"/>
      </xsl:call-template>
      <xsl:if test="string-length($tstyle/*[@name='separator']/@value) > 0
		    and string-length(@xtr:lab-end-label)>0">
	<xsl:value-of select="$tstyle/*[@name='separator']/@value"/>
	<xsl:call-template name="xtr-format-label">
	  <xsl:with-param name="config-xml" select="$config-xml"/>
	  <xsl:with-param name="format" select="@xtr:lab-end-label"/>
	  <xsl:with-param name="opener" select="@xtr:lab-start-label"/>
	</xsl:call-template>
      </xsl:if>
      <xsl:if test="string-length(@xtr:lab-start-label)>0">
	<xsl:value-of select="$tstyle/*[@name='closer']/@value"/>
      </xsl:if>
    </xsl:when>
    <xsl:otherwise>
      <xsl:choose>
	<xsl:when test="string-length(ancestor-or-self::*/@xtr:lab-start-label)">
	  <xsl:text>(</xsl:text>
	  <xsl:value-of select="ancestor-or-self::*/@xtr:lab-start-label"/>
	  <xsl:text>)</xsl:text>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="ancestor-or-self::*[@xtr:label][1]/@xtr:label"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="xtr-format-label">
  <xsl:param name="config-xml" select="''"/>
  <xsl:param name="format" select="@xtr:label"/>
  <xsl:param name="opener" select="''"/>
  <xsl:choose>
    <xsl:when test="contains($format, ' ') and contains($opener, ' ')">
      <xsl:choose>
	<xsl:when test="substring-before($format,' ')=substring-before($opener,' ')">
	  <xsl:variable name="format-sans-surface" select="substring-after($format,' ')"/>
	  <xsl:variable name="opener-sans-surface" select="substring-after($opener,' ')"/>
	  <xsl:choose>
	    <xsl:when test="contains($format-sans-surface,' ')
			    and contains($opener-sans-surface,' ')">
	      <xsl:choose>
		<xsl:when test="substring-before($format-sans-surface,' ')
				= substring-before($opener-sans-surface,' ')">
		  <xsl:call-template name="xtr-render-label">
		    <xsl:with-param name="label" 
				    select="substring-after($format-sans-surface,' ')"/>
		  </xsl:call-template>		  
		</xsl:when>
		<xsl:otherwise>
		  <xsl:call-template name="xtr-render-label">
		    <xsl:with-param name="label" select="$format-sans-surface"/>
		  </xsl:call-template>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:call-template name="xtr-render-label">
		<xsl:with-param name="label" select="$format-sans-surface"/>
	      </xsl:call-template>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:call-template name="xtr-format-surface">
	    <xsl:with-param name="config-xml" select="$config-xml"/>
	    <xsl:with-param name="format" select="$format"/>
	  </xsl:call-template>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="xtr-format-surface">
	<xsl:with-param name="config-xml" select="$config-xml"/>
	<xsl:with-param name="format" select="$format"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="xtr-format-surface">
  <xsl:param name="config-xml" select="''"/>
  <xsl:param name="format" select="@xtr:label"/>
  <xsl:variable name="label-surface">
    <xsl:call-template name="xpd-label">
      <xsl:with-param name="config-xml" select="$config-xml"/>
      <xsl:with-param name="label-surface" select="substring-before($format,' ')"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="string-length($label-surface) > 0">
      <xsl:call-template name="xtr-render-label">
	<xsl:with-param name="label" select="concat($label-surface, ' ', 
					     substring-after($format,' '))"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="xtr-render-label">
	<xsl:with-param name="label" select="$format"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="xtr-render-label">
  <xsl:param name="label"/>
  <xsl:variable name="faux-prime">'_ </xsl:variable>
  <xsl:variable name="real-prime">′&#xa0;&#xa0;</xsl:variable>
  <xsl:value-of select="translate($label,$faux-prime,$real-prime)"/>
</xsl:template>

</xsl:stylesheet>
