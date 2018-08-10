<?xml version='1.0'?>
<xsl:stylesheet version="1.0" 
		xmlns="http://www.w3.org/1999/xhtml"
		xmlns:esp="http://oracc.org/ns/esp/1.0"
		xmlns:lex="http://oracc.org/ns/lex/1.0"
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="lex:word-phra-data">
  <body>
    <xsl:apply-templates/>
  </body>
</xsl:template>

<xsl:template match="lex:group[@type='word']">
  <div class="lex-word" title="{@value}">
    <h3 class="word-base">Lexical Data</h3>
    <xsl:apply-templates/>
  </div>
</xsl:template>

<xsl:template match="lex:group[@type='phra']">
  <div class="lex-phra" title="{@value}">
    <h2 class="lex-phra"><a href="/{descendant::lex:data/@project}/{@lang}?xis={@xis}"
			    ><xsl:value-of select="@value"/></a></h2>
    <xsl:for-each select="*"> <!-- equi groups -->
      <!-- <p class="lex-equi"> --> <!-- a para for each equiv and its refs -->
	<!--
	<xsl:if test="not(@value='#none')">
	  <span class="lex-equi">
	    <xsl:text>= </xsl:text>
	    <i><xsl:value-of select="{@value}"/></i>
	  </span>
	  </xsl:if>
	  -->
	<xsl:for-each select="*"> <!-- line groups -->
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
      </xsl:for-each>
  </div>
</xsl:template>

<xsl:template name="emit-line">
  <xsl:for-each select="*[1]/*[1]">
    <span class="lex-line">
      <xsl:value-of select="*[1]/@form"/>
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
