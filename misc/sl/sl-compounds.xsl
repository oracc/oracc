<xsl:stylesheet
    xmlns:g="http://oracc.org/ns/gdl/1.0"
    xmlns:sl="http://oracc.org/ns/sl/1.0"
    xmlns:x="http://oracc.org/ns/xtf/1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0">

<xsl:output method="text" encoding="utf-8"/>

<xsl:include href="gdl-OATF.xsl"/>

<xsl:key name="signs" match="sl:sign|sl:pname|sl:form" use="@n"/>
<xsl:key name="pname" match="sl:pname" use="@n"/>
<xsl:key name="values" match="sl:v" use="@n"/>

<xsl:template match="/">
  <xsl:apply-templates select=".//g:c"/>
</xsl:template>

<xsl:template match="g:c">
  <xsl:variable name="res">
    <xsl:call-template name="sign-node">
      <xsl:with-param name="n" select="@form"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:value-of select="ancestor::x:l/@n"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:value-of select="@form"/>
  <xsl:text>&#x9;</xsl:text>
  <xsl:choose>
    <xsl:when test="string-length($res) > 0">
      <xsl:text>OK</xsl:text> <!-- c/@form is a known sign name -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="new-form">
	<xsl:text>|</xsl:text>
	<xsl:apply-templates/>
	<xsl:text>|</xsl:text>
      </xsl:variable>
      <xsl:variable name="new-res">
	<xsl:call-template name="sign-node">
	  <xsl:with-param name="n" select="$new-form"/>
	</xsl:call-template>
      </xsl:variable>
      <xsl:choose>
	<xsl:when test="string-length($res) > 0 and $res = $new-res">
	  <xsl:text>OK=</xsl:text><xsl:value-of select="$new-form"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:choose>
	    <xsl:when test="string-length($new-res) > 0">
	      <xsl:text>OK~</xsl:text><xsl:value-of select="$new-form"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text>NO:</xsl:text><xsl:value-of select="$new-form"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
  <xsl:text>&#xa;</xsl:text>
</xsl:template>

<xsl:template match="g:s">
  <xsl:variable name="body">
    <xsl:choose>
      <xsl:when test="g:b">
	<xsl:value-of select="g:b"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="."/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:variable name="vbody" select="translate($body,$xupper,$xlower)"/>
  <xsl:variable name="res">
    <xsl:call-template name="value-node">
      <xsl:with-param name="n" select="$vbody"/>
    </xsl:call-template>
  </xsl:variable>
  <xsl:choose>
    <xsl:when test="string-length($res)>0">
      <xsl:for-each select="document('file:///Users/stinney/orc/xml/ogsl/ogsl.xml')/*">
	<xsl:value-of select="translate(id($res)/@n,'|','')"/>
      </xsl:for-each>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$body"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="sign-node">
  <xsl:param name="n"/>
  <xsl:for-each select="document('file:///Users/stinney/orc/xml/ogsl/ogsl.xml')/*">
    <xsl:variable name="hits" select="key('signs',$n)|key('signs',translate($n,'()','')"/>
    <xsl:for-each select="$hits[1]">
      <xsl:choose>
	<xsl:when test="@xml:id">
	  <xsl:message><xsl:value-of select="name()"/> returning <xsl:value-of select="@xml:id"/></xsl:message>
	  <xsl:value-of select="key('signs',$n)/@xml:id"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:message><xsl:value-of select="name()"/> returning <xsl:value-of
	  select="ancestor::sl:sign/@xml:id"/></xsl:message>
	  <xsl:value-of select="ancestor::sl:sign/@xml:id"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

<xsl:template name="value-node">
  <xsl:param name="n"/>
  <xsl:for-each select="document('file:///Users/stinney/orc/xml/ogsl/ogsl.xml')/*">
    <xsl:for-each select="key('values',$n)[1]">
      <xsl:value-of select="ancestor::sl:sign/@xml:id"/>
    </xsl:for-each>
  </xsl:for-each>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
