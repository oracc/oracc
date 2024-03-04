<xsl:stylesheet version="1.0" 
		xmlns:sl="http://oracc.org/ns/sl/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:param name="wrap" select="'yes'"/>

<xsl:output method="text"/>

<xsl:template match="sl:signlist">
  <xsl:choose>
    <xsl:when test="$wrap = 'no'">
      <xsl:apply-templates/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:text>\macrofile{/Users/stinney/oracc/misc/sl/repmac}\signlist{Gudea Signlist}</xsl:text>
      <xsl:apply-templates/>
      <xsl:text>\endsignlist\newpage</xsl:text>
<!--
      <xsl:text>\signlist{Ur III Numbers}</xsl:text>
      <xsl:text>\include{ur3-num}</xsl:text>
      <xsl:text>\endsignlist\newpage</xsl:text>
 -->
      <xsl:text>\include{valindex}&#xa;</xsl:text>
      <xsl:text>\bye</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="sl:sign">
  <xsl:variable name="kwu-name">
    <xsl:choose>
      <xsl:when test="count(sl:list[starts-with(@n,'KWU')])>0">
	<xsl:for-each select="sl:list[starts-with(@n, 'KWU')][1]">
	  <xsl:value-of select="@n"/>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:for-each select="sl:list[starts-with(@n, 'MZL')][1]">
	  <xsl:value-of select="@n"/>
	</xsl:for-each>
      </xsl:otherwise>
    </xsl:choose>	
  </xsl:variable>
  <xsl:variable name="values">
    <xsl:choose>
      <xsl:when test="sl:v">
	<xsl:for-each select="sl:v">
	  <xsl:value-of select="concat('\hbox{', text(), ' (', @icount, ')\hfil}')"/>
	  <xsl:if test="not(position() = last())">
	    <xsl:text>, </xsl:text>
	  </xsl:if>
	</xsl:for-each>
      </xsl:when>
      <xsl:otherwise>
	<xsl:value-of select="concat('\hbox{{\eight\rm ',@n,'} (', @icount,')\hfil}')"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:variable>
  <xsl:value-of select="concat('\repsign{',sl:utf8/@utf8,'}{',@n,'}{',$kwu-name,'}{', $values, '}{', @slnum, '}&#xa;')"/>
</xsl:template>

<xsl:template match="text()"/>

</xsl:stylesheet>
