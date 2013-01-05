<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
  xmlns:bib="http://oracc.org/ns/bib/1.0"
  xmlns="http://www.tei-c.org/ns/1.0"
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:exslt="http://exslt.org/common"
  extension-element-prefixes="exslt t"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="sort-bib.xsl"/>

<xsl:key name="name-date" match="t:biblStruct" 
	 use="concat(@ahash,' ',*/t:imprint/t:date)"/>

<xsl:key name="same-last" match="t:biblStruct[@bib:same-auth1='f']"
	 use="@bib:last"/>

<xsl:template match="t:listBibl">
  <xsl:param name="nl" select="*"/>
  <xsl:variable name="sorted">
    <xsl:call-template name="sortbib"/>
  </xsl:variable>
  <xsl:variable name="interim">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:for-each select="exslt:node-set($sorted)/bibWrapper">
	<xsl:variable name="ahash" select="@ahash"/>
	<xsl:variable name="last" select="@bib:last"/>
	<xsl:variable name="first" select="@bib:first"/>
	<xsl:variable name="init" select="@bib:init"/>
	<xsl:variable name="same-auth">
	  <xsl:call-template name="calc-same-auth"/>
	</xsl:variable>
	<xsl:variable name="same-auth1">
	  <xsl:call-template name="calc-same-auth1"/>
	</xsl:variable>
	<xsl:variable name="index" select="@index"/>
	<xsl:for-each select="$nl[number($index)]">
	  <xsl:copy>
	    <xsl:attribute name="bib:same-auth">
	      <xsl:value-of select="$same-auth"/>
	    </xsl:attribute>
	    <xsl:attribute name="bib:same-auth1">
	      <xsl:value-of select="$same-auth1"/>
	    </xsl:attribute>
	    <xsl:attribute name="bib:ahash">
	      <xsl:value-of select="$ahash"/>
	    </xsl:attribute>
	    <xsl:attribute name="bib:last">
	      <xsl:value-of select="$last"/>
	    </xsl:attribute>
	    <xsl:attribute name="bib:init">
	      <xsl:value-of select="$init"/>
	    </xsl:attribute>
	    <xsl:attribute name="bib:first">
	      <xsl:value-of select="$first"/>
	    </xsl:attribute>
	    <xsl:copy-of select="@*"/>
	    <xsl:copy-of select="*"/>
	  </xsl:copy>
	</xsl:for-each>
      </xsl:for-each>
    </xsl:copy>
  </xsl:variable>
  <xsl:for-each select="exslt:node-set($interim)/*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:for-each select="*">
	<xsl:variable name="name-date" 
		  select="concat(@ahash,' ',*/*/t:date)"/>
	<xsl:variable name="name-date-attr">
	  <xsl:choose>
	    <xsl:when test="count(key('name-date',$name-date))>1">
	      <xsl:variable name="ahash" select="@ahash"/>
	      <xsl:variable name="date" select="*/t:imprint/t:date"/>
	      <xsl:value-of select="$date"/>
	      <xsl:number format="a" 
			  select="count(preceding-sibling::*
				  [@ahash=$ahash]
				  [*/t:imprint/t:date=$date])"/>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:value-of select="$name-date"/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:variable name="multi">
	  <xsl:choose>
	    <xsl:when test="@bib:same-auth1='f'">
	      <xsl:choose>
		<xsl:when test="count(key('same-last',@bib:last))>1">
		  <!--<xsl:message>multi-test</xsl:message>-->
		  <xsl:text>t</xsl:text>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:text/>
		</xsl:otherwise>
	      </xsl:choose>
	    </xsl:when>
	    <xsl:otherwise>
	      <xsl:text/>
	    </xsl:otherwise>
	  </xsl:choose>
	</xsl:variable>
	<xsl:copy>
	  <xsl:copy-of select="@*"/>
	  <xsl:attribute name="bib:date">
	    <xsl:value-of select="*/t:imprint/t:date"/>
	  </xsl:attribute>
	  <xsl:if test="$multi='t'">
	    <xsl:attribute name="bib:multi-last">
	      <xsl:text>t</xsl:text>
	    </xsl:attribute>
	  </xsl:if>
	  <xsl:copy-of select="*"/>
	</xsl:copy>
      </xsl:for-each>
    </xsl:copy>
  </xsl:for-each>
</xsl:template>


<xsl:template name="calc-same-auth">
  <xsl:variable name="prv" select="preceding-sibling::*[1]"/>
  <xsl:choose>
    <xsl:when test="@ahash = $prv/@ahash">
      <xsl:value-of select="'t'"/>
    </xsl:when>
    <xsl:otherwise>
       <xsl:value-of select="'f'"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="calc-same-auth1">
  <xsl:variable name="prv" select="preceding-sibling::*[1]"/>
  <xsl:choose>
    <xsl:when test="@akey1 = $prv/@akey1">
      <xsl:value-of select="'t'"/>
    </xsl:when>
    <xsl:otherwise>
       <xsl:value-of select="'f'"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
