<xsl:stylesheet version="1.0" 
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:exslt="http://exslt.org/common"
  xmlns:bib="http://oracc.org/ns/bib/1.0"
  extension-element-prefixes="exslt"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="xabbrevs" 
            select="document('/home/s/xabbrevs.xml')/*/*"/>

<xsl:template match="reflist">
  <citekeys>
  <xsl:for-each select="*">
    <xsl:variable name="abbrev">
      <xsl:call-template name="get-abbrev"/>
    </xsl:variable>
    <xsl:variable name="last-name">
      <xsl:call-template name="get-last-name"/>
    </xsl:variable>
    <ckey bib:id="{@bib:id}">
      <names><xsl:value-of select="$last-name"/></names>
      <abbrev><xsl:value-of select="$abbrev"/></abbrev>
    </ckey>
  </xsl:for-each>
  </citekeys>    
</xsl:template>

<xsl:template name="get-abbrev">
  <xsl:choose>
    <xsl:when test="t:note[@type='abbrev']">
      <xsl:value-of select="t:note[@type='abbrev']"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:variable name="db" select="@bib:db"/>
      <xsl:variable name="id" select="@id"/>
      <xsl:choose>
        <xsl:when test="$xabbrevs[@db=$db and @id=$id]">
          <xsl:value-of select="$xabbrevs[@db=$db and @id=$id]"/>
        </xsl:when>
        <xsl:otherwise>
          <xsl:message><xsl:value-of 
                select="@ref:db"/>:<xsl:value-of 
                select="@id"/>: no abbrev</xsl:message>
          <xsl:value-of select="string(.//title[1])"/>
        </xsl:otherwise>
      </xsl:choose>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="get-last-name">
  <xsl:variable name="au" select=".//author[1]"/>
  <xsl:choose>
    <xsl:when test="contains($au, ',')">
      <xsl:value-of select="substring-before($au, ',')"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$au"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
