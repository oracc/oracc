<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="lex:phra-base-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lex:group[@type='word']">
  <div class="lex-word" title="{@value}">
    <h3 class="lex-base">Pronunciation Data</h3>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='base']">
  <div class="lex-base" title="{@value}">
    <!-- href="/{descendant::lex:data/@project}/{@lang}?xis={../@xis}" -->
    <h2 class="lex-base"><a href="javascript:distprof2('{/*/@project}','{@lang}','{../@xis}')"
			    ><xsl:value-of select="@value"/></a></h2>
    <xsl:for-each select="*"> <!-- spel groups -->
      <p class="lex-line">
	<xsl:call-template name="emit-line"/>
	<xsl:text> (</xsl:text>
	<xsl:for-each select="*"> <!-- ref groups -->
	  <xsl:apply-templates select="."/>
	  <xsl:if test="not(position()=last())">
	    <xsl:text>; </xsl:text>
	  </xsl:if>
	</xsl:for-each>
	<xsl:text>).</xsl:text>
      </p>
    </xsl:for-each>
  </div>
</xsl:template>

<xsl:template name="emit-line">
  <xsl:for-each select="lex:group[1]/lex:data[1]">
    <span class="lex-spel">
      <xsl:value-of select="@spel"/>
      <xsl:if test="string-length(@read) > 0">
	<xsl:text> = </xsl:text>
	<i><xsl:value-of select="@read"/></i>
      </xsl:if>
      <xsl:if test="string-length(lex:eq/@form) > 0">
	<xsl:text> = </xsl:text>
	<i><xsl:value-of select="lex:eq/@form"/></i>
      </xsl:if>
    </span>
  </xsl:for-each>
</xsl:template>

<xsl:template match="lex:group[@type='refs']">
  <xsl:variable name="wrefs">
    <xsl:for-each select="lex:data/lex:eq/*/@wref">
      <xsl:value-of select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>+</xsl:text>
      </xsl:if>
    </xsl:for-each>
  </xsl:variable>
  <xsl:variable name="proj" select="lex:data[1]/@project"/>
  <a href="http://oracc.org/{$proj}/{$wrefs}" title="{$proj} on Oracc">
    <xsl:value-of select="lex:data[1]/@n"/>
    <xsl:text> </xsl:text>
    <xsl:for-each select="lex:data/@label">
      <xsl:value-of select="."/>
      <xsl:if test="not(position()=last())">
	<xsl:text>, </xsl:text>
      </xsl:if>
    </xsl:for-each>
  </a>
</xsl:template>

</xsl:stylesheet>
