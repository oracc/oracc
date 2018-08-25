<xsl:stylesheet version="1.0" 
		xmlns:c="http://oracc.org/ns/cbd/1.0"
		xmlns:x="http://oracc.org/ns/xis/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:key name="refgroups" match="x:r"
	 use="concat(../@xml:id,':',@id_text)"/>

<xsl:template match="c:form">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates mode="refs"
   		         select="x:r[generate-id(.)
		 	                  =generate-id(
					   key('refgroups',
					       concat(../@xml:id,':',@id_text)))
					   ]"/>
  </xsl:copy>
</xsl:template>

<xsl:template match="x:r" mode="refs">
  <x:rr value="{@id_text}">
    <xsl:for-each select="key('refgroups', concat(../@xml:id,':',@id_text))">
      <xsl:for-each select=".">
	<xsl:copy>
	  <xsl:copy-of select="@*[not(name()='id_text')]"/>
	  <xsl:value-of select="."/>
	</xsl:copy>
      </xsl:for-each>
    </xsl:for-each>
  </x:rr>
</xsl:template>

<xsl:template match="*">
  <xsl:copy>
    <xsl:copy-of select="@*"/>
    <xsl:apply-templates/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
