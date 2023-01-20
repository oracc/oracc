<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	       xmlns="http://oracc.org/ns/sl/1.0"
	       xmlns:sl="http://oracc.org/ns/sl/1.0"
	       >

<xsl:key name="glo" match="sl:val" use="@n"/>
  
<xsl:template match="sl:sign">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:copy-of select="*[not(local-name()='v')]"/>
    <xsl:variable name="n" select="@n"/>
    <xsl:variable name="sname" select="sl:name"/>
    <xsl:for-each select="document('signdata.xml',/)">
      <xsl:variable name="glo" select="key('glo',$n)"/>
      <xsl:if test="$glo/sl:glo-group">
	<v n="{$n}">
	  <xsl:copy-of select="$sname"/> <!--"*[not(local-name()='v')]"/> -->
	  <glo>
	    <xsl:copy-of select="$glo/*"/>
	  </glo>
	</v>
      </xsl:if>
    </xsl:for-each>
    <xsl:for-each select="sl:v">
      <xsl:copy>
	<xsl:copy-of select="@*"/>
	<xsl:copy-of select="*"/>
	<xsl:variable name="vn" select="@n"/>
	<xsl:for-each select="document('signdata.xml',/)">
	  <xsl:variable name="glo" select="key('glo',$vn)"/>
	  <xsl:if test="$glo/sl:glo-group">
	    <glo>
	      <xsl:copy-of select="$glo/*"/>
	    </glo>
	  </xsl:if>
	</xsl:for-each>
      </xsl:copy>
    </xsl:for-each>
  </xsl:copy>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:transform>  
