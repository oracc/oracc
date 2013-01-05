<?xml version='1.0'?>

<!-- Implement a mostly Chicago Manual of Style type A reference list
     for TEI biblStruct.

     Series formatting doesn't conform; I prefer series name and
     volume in parentheses with no `no.' or `vol.'

     Could use a `strict' mode to support CMS type A more properly;
     would require bibliographies to be marked up explicitly w/r/t 
     volume or number.

     Could use support for <bibl>; just means an alternate top-level
     format routine, and a test to detect which formatter to use based
     on tag name.
 -->

<!DOCTYPE xsl:stylesheet [
<!ENTITY newline "<xsl:text xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>&#xa;</xsl:text>">
]>
<xsl:stylesheet version="1.0" 
  xmlns="http://www.w3.org/1999/xhtml"
  xmlns:h="http://www.w3.org/1999/xhtml"
  xmlns:bib="http://oracc.org/ns/bib/1.0"
  xmlns:t="http://www.tei-c.org/ns/1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:include href="map-jrnser.xsl"/>

<xsl:variable name="lq" select="'&#x201c;'"/>
<xsl:variable name="rq" select="'&#x201d;'"/>

<xsl:template name="format-biblStruct">
  <xsl:choose>
    <xsl:when test="t:analytic/t:title and t:monogr/t:title">
      <xsl:call-template name="format-article">
        <xsl:with-param name="same-auth" select="@bib:same-auth"/>
	<xsl:with-param name="aut" select="t:analytic/t:author"/>
	<xsl:with-param name="edi" select="t:monogr/t:editor"/>
	<xsl:with-param name="art" select="t:analytic/t:title"/>
	<xsl:with-param name="boo" select="t:monogr/t:title"/>
	<xsl:with-param name="vol" 
			select="t:monogr/t:biblScope[@type='vol']|
				t:monogr/t:imprint/biblScope[@type='vol']"/>
	<xsl:with-param name="pag"
                        select="t:monogr/t:biblScope[@type='pages']
				|t:monogr/t:imprint/t:biblScope[@type='pages']"/>
	<xsl:with-param name="sn1" select="t:series[1]/t:title"/>
	<xsl:with-param name="sv1" 
			select="t:series[1]/t:biblScope[@type='number']"/>
	<xsl:with-param name="sn2" select="t:series[2]/t:title"/>
	<xsl:with-param name="sv2" 
			select="t:series[2]/t:biblScope[@type='number']"/>
	<xsl:with-param name="pub" select="t:monogr/t:imprint/t:publisher"/>
	<xsl:with-param name="pla" select="t:monogr/t:imprint/t:pubPlace"/>
	<xsl:with-param name="dat" select="t:monogr/t:imprint/t:date"/>
	<xsl:with-param name="not" select="t:note"/>
        <xsl:with-param name="typ" select="@type"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="format-book">
        <xsl:with-param name="same-auth" select="@bib:same-auth"/>
	<xsl:with-param name="aut" select="t:analytic/t:author|t:monogr/t:author"/>
	<xsl:with-param name="edi" select="t:analytic/t:editor|t:monogr/t:editor"/>
	<xsl:with-param name="boo" select="t:analytic/t:title|t:monogr/t:title"/>
	<xsl:with-param name="sn1" select="t:series[1]/t:title"/>
	<xsl:with-param name="sv1" 
			select="t:series[1]/t:biblScope[@type='number']
				|t:series[1]/t:biblScope[@type='vol']"/>
	<xsl:with-param name="sn2" select="t:series[2]/t:title"/>
	<xsl:with-param name="sv2" 
			select="t:series[2]/t:biblScope[@type='number']
				|t:series[2]/t:biblScope[@type='vol']"/>
	<xsl:with-param name="pub" select="t:monogr/t:imprint/t:publisher"/>
	<xsl:with-param name="pla" select="t:monogr/t:imprint/t:pubPlace"/>
	<xsl:with-param name="dat" select="t:monogr/t:imprint/t:date"/>
	<xsl:with-param name="not" select="t:note"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="format-article">
  <xsl:param name="same-auth"/>
  <xsl:param name="aut"/>
  <xsl:param name="edi"/>
  <xsl:param name="art"/>
  <xsl:param name="boo"/>
  <xsl:param name="vol"/>
  <xsl:param name="pag"/>
  <xsl:param name="sn1"/>
  <xsl:param name="sv1"/>
  <xsl:param name="sn2"/>
  <xsl:param name="sv2"/>
  <xsl:param name="pub"/>
  <xsl:param name="pla"/>
  <xsl:param name="dat"/>
  <xsl:param name="not"/>

  <p class="article">

  <xsl:choose>
    <xsl:when test="$same-auth = 't'">
      <xsl:text>---------. </xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="fmt-names">
        <xsl:with-param name="names" select="$aut"/>
      </xsl:call-template>
      <xsl:call-template name="maybe-add-period">
        <xsl:with-param name="prev-str" select="$aut[last()]"/>
      </xsl:call-template>
      <xsl:text> </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  &newline;

  <xsl:value-of select="$lq"/>
    <xsl:value-of select="normalize-space($art)"/>
  <xsl:call-template name="maybe-add-period">
    <xsl:with-param name="prev-str" select="$art"/>
  </xsl:call-template>
  <xsl:value-of select="$rq"/>
  <xsl:text> </xsl:text>
  &newline;

  <xsl:choose>
    <xsl:when test="string-length($vol)=0">
      <xsl:text>Pp. </xsl:text>
      <xsl:value-of select="$pag"/>
      <xsl:text> in </xsl:text>
      <xsl:call-template name="fmt-bookinfo">
        <xsl:with-param name="aut" select="$aut"/>
        <xsl:with-param name="edi" select="$edi"/>
        <xsl:with-param name="boo" select="$boo"/>
        <xsl:with-param name="sn1" select="$sn1"/>
        <xsl:with-param name="sv1" select="$sv1"/>
        <xsl:with-param name="sn2" select="$sn2"/>
        <xsl:with-param name="sv2" select="$sv2"/>
        <xsl:with-param name="pub" select="$pub"/>
        <xsl:with-param name="pla" select="$pla"/>
        <xsl:with-param name="dat" select="$dat"/>
        <xsl:with-param name="not" select="$not"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <span class="journal">
        <xsl:call-template name="map-jrnser">
          <xsl:with-param name="js" select="$boo"/>
        </xsl:call-template>
      </span>
      <xsl:text> </xsl:text>
      <xsl:if test="$vol">
        <xsl:value-of select="$vol"/>
      </xsl:if>
      <xsl:if test="$dat">
        <xsl:text> (</xsl:text>
        <xsl:value-of select="$dat"/>
        <xsl:text>)</xsl:text>
      </xsl:if>
      <xsl:if test="$pag">
        <xsl:text>: </xsl:text>
        <xsl:value-of select="$pag"/>
      </xsl:if>
      <xsl:text>.</xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  </p>
</xsl:template>

<xsl:template name="format-book">
  <xsl:param name="same-auth"/>
  <xsl:param name="aut"/>
  <xsl:param name="edi"/>
  <xsl:param name="boo"/>
  <xsl:param name="sn1"/>
  <xsl:param name="sv1"/>
  <xsl:param name="sn2"/>
  <xsl:param name="sv2"/>
  <xsl:param name="pub"/>
  <xsl:param name="pla"/>
  <xsl:param name="dat"/>
  <xsl:param name="not"/>

  <p class="book">

  <xsl:choose>
    <xsl:when test="$same-auth = 't'">
      <xsl:text>---------. </xsl:text>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="fmt-author">
        <xsl:with-param name="aut" select="$aut"/>
        <xsl:with-param name="edi" select="$edi"/>
        <xsl:with-param name="pub" select="$pub"/>
      </xsl:call-template>
      <xsl:text> </xsl:text>
    </xsl:otherwise>
  </xsl:choose>
  &newline;

  <xsl:call-template name="fmt-bookinfo">
    <xsl:with-param name="aut" select="$aut"/>
    <xsl:with-param name="edi" select="$edi"/>
    <xsl:with-param name="boo" select="$boo"/>
    <xsl:with-param name="sn1" select="$sn1"/>
    <xsl:with-param name="sv1" select="$sv1"/>
    <xsl:with-param name="sn2" select="$sn2"/>
    <xsl:with-param name="sv2" select="$sv2"/>
    <xsl:with-param name="pub" select="$pub"/>
    <xsl:with-param name="pla" select="$pla"/>
    <xsl:with-param name="dat" select="$dat"/>
    <xsl:with-param name="not" select="$not"/>
  </xsl:call-template>
  </p>
</xsl:template>

<xsl:template name="fmt-bookinfo">
  <xsl:param name="aut"/>
  <xsl:param name="edi"/>
  <xsl:param name="boo"/>
  <xsl:param name="sn1"/>
  <xsl:param name="sv1"/>
  <xsl:param name="sn2"/>
  <xsl:param name="sv2"/>
  <xsl:param name="pub"/>
  <xsl:param name="pla"/>
  <xsl:param name="dat"/>
  <xsl:param name="not"/>

  <xsl:variable name="norm-boo" select="normalize-space($boo)"/>

  <span class="booktitle"><xsl:value-of select="$norm-boo"/>
    <xsl:call-template name="maybe-add-period">
      <xsl:with-param name="prev-str" select="$norm-boo"/>
    </xsl:call-template>
  </span>
  <xsl:if test="$edi and $aut">
    <xsl:text> </xsl:text>
    &newline;
    <xsl:call-template name="fmt-names">
      <xsl:with-param name="names" select="$edi"/>
      <xsl:with-param name="flags" select="1"/>
    </xsl:call-template>
  </xsl:if>

  <xsl:if test="$sn1">
    <xsl:text> </xsl:text>
    &newline;
    <xsl:text>(</xsl:text>
    <xsl:call-template name="map-jrnser">
      <xsl:with-param name="js" select="$sn1"/>
    </xsl:call-template>
    <xsl:text> </xsl:text>
    <xsl:value-of select="$sv1"/>
    <xsl:if test="$sn2">
      <xsl:text> = </xsl:text>
      <xsl:call-template name="map-jrnser">
        <xsl:with-param name="js" select="$sn2"/>
      </xsl:call-template>
      <xsl:text> </xsl:text>
      <xsl:value-of select="$sv2"/>
    </xsl:if>
    <xsl:text>). </xsl:text>
  </xsl:if>

  <xsl:call-template name="trap-diss">
    <xsl:with-param name="strs" select="$not"/>
  </xsl:call-template>

  <xsl:if test="$pub or $pla or $dat">
    <xsl:text> </xsl:text>
    &newline;
    <xsl:value-of select="$pla"/>
    <xsl:if test="$pub">
      <xsl:if test="$pla">
        <xsl:text>: </xsl:text>
      </xsl:if>
      <xsl:value-of select="$pub"/>
    </xsl:if>
    <xsl:if test="$dat">
      <xsl:if test="$pub or $pla">
        <xsl:text>, </xsl:text>
      </xsl:if>
      <xsl:value-of select="$dat"/>
    </xsl:if>
    <xsl:text>. </xsl:text>
  </xsl:if>

</xsl:template>

<xsl:template name="fmt-author">
  <xsl:param name="aut"/>
  <xsl:param name="edi"/>
  <xsl:param name="pub"/>
  <xsl:choose>
    <xsl:when test="count($aut) > 0">
      <xsl:call-template name="fmt-names">
        <xsl:with-param name="names" select="$aut"/>
      </xsl:call-template>
      <xsl:call-template name="maybe-add-period">
        <xsl:with-param name="prev-str" select="$aut[last()]"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:when test="count($edi) > 0">
      <xsl:call-template name="fmt-names">
        <xsl:with-param name="names" select="$edi"/>
        <xsl:with-param name="flags" select="1"/>
      </xsl:call-template>
<!--
      <xsl:call-template name="maybe-add-period">
        <xsl:with-param name="prev-str" select="$edi[last()]"/>
      </xsl:call-template>
 -->
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="fmt-names">
        <xsl:with-param name="names" select="$pub"/>
        <xsl:with-param name="flags" select="2"/>
      </xsl:call-template>
      <xsl:call-template name="maybe-add-period">
        <xsl:with-param name="prev-str" select="$pub[last()]"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="fmt-names">
  <xsl:param name="names"/>
  <xsl:param name="flags" select="0"/>

  <xsl:for-each select="$names">
    <xsl:choose>
      <xsl:when test="position() > 1 and position() = last()">
        <xsl:text> and </xsl:text>
      </xsl:when>
      <xsl:when test="position() > 1">
        <xsl:text>, </xsl:text>
      </xsl:when>
    </xsl:choose>
    <xsl:value-of select="normalize-space(.)"/>
  </xsl:for-each>
  <xsl:if test="$flags = 1">
    <xsl:text>, ed</xsl:text>
    <xsl:if test="count($names) > 1">
      <xsl:text>s</xsl:text>
    </xsl:if>
    <xsl:text>. </xsl:text>
  </xsl:if>
</xsl:template>

<xsl:template name="maybe-add-period">
  <xsl:param name="prev-str"/>
  <xsl:variable name="str" select="normalize-space($prev-str)"/>
  <xsl:if test="translate(substring($str,
                                    string-length($str)),
                          ',.:;?!',
                          '')">
    <xsl:text>.</xsl:text>
  </xsl:if>
</xsl:template>

<!-- 
 Try to spot Ph.D./M.A./D.Phil. dissertations/theses in a plain text
 string.  Dump the concatenation of the string nodes if no diss is found.
 -->
<xsl:template name="trap-diss">
  <xsl:param name="strs"/>
  <xsl:variable name="onestr">
    <xsl:for-each select="$strs">
      <xsl:if test="not(@type='abbrev') and not(@type='keys')">
        <xsl:value-of select="."/>
        <xsl:call-template name="maybe-add-period">
          <xsl:with-param name="prev-str" select="."/>
        </xsl:call-template>
        <xsl:text> </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="norm-str" select="translate(translate($onestr,
						            '.-',
							    ''),
						  'PHDMADISSLTE',
					   	  'phdmadisslte')"/>
  <xsl:if test="contains($norm-str,'diss') 
             or contains($norm-str,'thesis')">
    <xsl:text> </xsl:text>
    <xsl:choose>
      <xsl:when test="contains($norm-str,'phd')
                   or contains($norm-str,'dphil')">
        <xsl:text>Ph.D. dissertation. </xsl:text>
      </xsl:when>
      <xsl:when test="contains($norm-str,'ma')">
        <xsl:text>M.A. thesis. </xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$onestr"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:if>
</xsl:template>

<!-- BUGS: 

  series handling may be inadequate; need to support more than two?

  how about a journal volume that is also in a series?

  -->

</xsl:stylesheet>
