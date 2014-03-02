<xsl:stylesheet version="1.0" 
		xmlns:sl="http://oracc.org/ns/sl/1"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:output method="text"/>

<xsl:template match="sl:signlist">
  <xsl:text>\macrofile{../tools/gslmac}\signlist{GSL}%
</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>\endsignlist
\bye
</xsl:text>
</xsl:template>

<xsl:template match="sl:sign">
  <xsl:value-of select="concat('\sign{',@n,'}&#xa;')"/>
  <xsl:call-template name="sign-data"/>
  <xsl:apply-templates select="sl:form"/>
  <xsl:text>\endsign
</xsl:text>
</xsl:template>

<xsl:template match="sl:form">
  <xsl:value-of select="concat('\form{',@n,'}{',@var,'}&#xa;')"/>
  <xsl:call-template name="sign-data"/>
  <xsl:text>\endform
</xsl:text>
</xsl:template>

<xsl:template name="sign-data">
  <xsl:call-template name="unicode-info"/>
  <xsl:if test="sl:list">
    <xsl:text>\lists{</xsl:text>
    <xsl:for-each select="sl:list">
      <xsl:value-of select="@n"/>
      <xsl:if test="@name">
	<xsl:value-of select="concat(' (',@name,')')"/>
      </xsl:if>
      <xsl:if test="not(position() = last())">
	<xsl:text>, </xsl:text>
      </xsl:if>
    </xsl:for-each>
    <xsl:text>}
</xsl:text>
  </xsl:if>
  <xsl:call-template name="notes"/>
  <xsl:call-template name="instances"/>
  <xsl:call-template name="values"/>
</xsl:template>

<xsl:template name="notes">
  <xsl:if test="sl:note">
    <xsl:text>\gslnotes
</xsl:text>
<xsl:for-each select="sl:note">
    <xsl:text>\gslnote{</xsl:text>
    <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
  </xsl:for-each>
  <xsl:text>\endgslnotes
</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="instances">
  <xsl:if test="sl:inst">
    <xsl:text>\instances
</xsl:text>
    <xsl:for-each select="sl:inst">
      <xsl:text>\inst{</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>}
</xsl:text>
    </xsl:for-each>
    <xsl:text>\endinstances
</xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="unicode-info">
  <xsl:value-of select="concat('\unicode{',sl:utf8/@hex,'}{',sl:utf8/text(),'}&#xa;')"/>
</xsl:template>

<xsl:template name="values">
  <xsl:text>\values
</xsl:text>
  <xsl:text>\vals{</xsl:text>
  <xsl:for-each select="sl:v">
    <xsl:value-of select="@n"/>
    <xsl:if test="not(position() = last())">
      <xsl:text>, </xsl:text>
    </xsl:if>
  </xsl:for-each>
  <xsl:text>}
</xsl:text>
  <xsl:call-template name="notes"/>
  <xsl:call-template name="instances"/>
  <xsl:text>\endvalues
</xsl:text>  
</xsl:template>

</xsl:stylesheet>
