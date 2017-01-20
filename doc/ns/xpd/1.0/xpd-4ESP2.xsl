<xsl:stylesheet version="1.0" 
		xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns="http://www.w3.org/1999/xhtml" 
		xmlns:xf="http://www.w3.org/2002/xforms"
		xmlns:ev="http://www.w3.org/2001/xml-events"
		xmlns:xs="http://www.w3.org/2001/XMLSchema"
		xmlns:a="http://relaxng.org/ns/compatibility/annotations/1.0"
		xmlns:d="http://oracc.org/ns/d"
		xmlns:m="http://oracc.org/ns/m"
		xmlns:rng="http://relaxng.org/ns/structure/1.0"
		xmlns:xpd="http://oracc.org/ns/xpd/1.0"
		xmlns:ex="http://exslt.org/common"
		extension-element-prefixes="ex"
		exclude-result-prefixes="a d m rng xf ev xs xpd">

<xsl:output method="xml" omit-xml-declaration="yes" indent="yes"/>

<xsl:template match="rng:grammar">
  <xsl:apply-templates mode="case" select="rng:div"/>
</xsl:template>

<xsl:template mode="case" match="rng:div">
  <ex:document href="./configesp2/{@m:name}.xml"
	       method="xml" encoding="utf-8" omit-xml-declaration="yes"
	       indent="yes">
    <div id="div-{@m:name}" class="optgroup">
      <h2><xsl:call-template name="tab-label"/> &#x2014;
	<span class="groupdoc"><xsl:value-of select="a:documentation[1]"/></span>
      </h2>
      <xsl:choose>
	<xsl:when test="rng:div">
	  <xsl:for-each select="rng:div">
	    <fieldset class="{@m:name}">
	      <dl>
		<xsl:apply-templates/>
	      </dl>
	    </fieldset>
	  </xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
	  <fieldset class="{@m:name}">
	    <dl>
	      <xsl:apply-templates/>
	    </dl>
	  </fieldset>
	</xsl:otherwise>
      </xsl:choose>
    </div>
  </ex:document>
</xsl:template>

<xsl:template match="rng:define">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="rng:element[@name='option']">
  <dt class="option">
    <xsl:value-of select="rng:attribute[@name='name']/rng:value"/>
    <xsl:text>: </xsl:text>
  </dt>
  <dd class="doco">
    <p>
      <xsl:for-each select="rng:attribute[@name='value']">
	<xsl:call-template name="doco"/>
      </xsl:for-each>
    </p>
    <xsl:call-template name="help"/>
  </dd>
</xsl:template>

<xsl:template match="rng:element[@name='n' or @name='type' or @name='public']"/>

<xsl:template match="rng:element">
  <dt class="option">
    <xsl:value-of select="ancestor::rng:define/@name"/>
    <xsl:text>: </xsl:text>
  </dt>
  <xsl:choose>
    <xsl:when test="rng:ref[@name='textarea']">
      <dd colspan="2" class="double">
	<p>
	  <!--
	      <xsl:call-template name="xf-input">
	      <xsl:with-param name="ref" select="$ref"/>
	      </xsl:call-template>
	      <br/>
	  -->
	  <xsl:call-template name="doco"/>
	</p>
      </dd>
    </xsl:when>
    <xsl:otherwise>
      <dd class="doco">
	<xsl:call-template name="doco"/>
      </dd>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template match="rng:attribute">
</xsl:template>

<xsl:template match="a:*"/>

<xsl:template match="text()"/>

<xsl:template name="doco">
  <xsl:choose>
    <xsl:when test="rng:choice">
      <xsl:for-each select="rng:choice/rng:value">
	<p class="choicedoc">
	  <span class="valval">
	    <xsl:value-of select="."/>
	  </span>
	  <xsl:text>: </xsl:text>
	  <span class="valdoc">
	    <xsl:value-of select="following-sibling::*[1][self::a:documentation]"/>
	  </span>
	</p>
      </xsl:for-each>
    </xsl:when>
    <xsl:when test="*/a:documentation">
      <span class="docsolo">
	<xsl:value-of select="*/a:documentation"/>
      </span>      
    </xsl:when>
    <xsl:otherwise>
      <span class="docsolo">
	<xsl:value-of select="a:documentation"/>
      </span>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="help">
  <pre class="help">
    <xsl:value-of select="a:documentation"/>
  </pre>
</xsl:template>

<xsl:template name="set-ref">
  <xsl:for-each select="ancestor-or-self::rng:element">
    <xsl:choose>
      <xsl:when test="@name='option'">
	<xsl:variable name="pre">
	  <xsl:text>instance('config')/xpd:option[@name='</xsl:text>
	</xsl:variable>
	<xsl:variable name="post">
	  <xsl:text>']/@value</xsl:text>
	</xsl:variable>
	<xsl:value-of select="concat($pre,
			             rng:attribute[@name='name']/rng:value,
				     $post)"/>
      </xsl:when>
      <xsl:otherwise>
	<xsl:text>instance('config')/xpd:</xsl:text>
	<xsl:value-of select="@name"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
</xsl:template>

<xsl:template name="tab-label">
  <xsl:choose>
    <xsl:when test="@m:tab">
      <xsl:value-of select="@m:tab"/>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="@m:name"/>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="make-blobs">
  <xsl:for-each select="//rng:element[*[1][self::a:documentation]]">
    <xsl:variable name="opt-name">
      <xsl:choose>
	<xsl:when test="@name='option'">
	  <xsl:value-of select="rng:attribute[@name='name']/rng:value"/>
	</xsl:when>
	<xsl:otherwise>
	  <xsl:value-of select="@name"/>
	</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <blob xmlns="" name="{$opt-name}">
      <xsl:value-of select="a:documentation[1]"/>
    </blob>
  </xsl:for-each>
</xsl:template>

<xsl:template name="xf-input">
  <xsl:param name="ref"/>
<!--
  <xsl:choose>
    <xsl:when test="rng:choice">
      <xf:select1>
	<xsl:attribute name="ref"><xsl:value-of select="$ref"/></xsl:attribute>
	<xsl:for-each select="rng:choice/rng:value">
	  <xf:item>
	    <xf:label><xsl:value-of select="."/></xf:label>
	    <xf:value><xsl:value-of select="."/></xf:value>
	  </xf:item>
	</xsl:for-each>
      </xf:select1>
    </xsl:when>
    <xsl:when test="rng:text or rng:data[@type='NCName' or 
		    			 @type='anyURI' or
					 @type='boolean']">
      <xf:input ref="{$ref}">
	<xf:label/>
      </xf:input>
    </xsl:when>
    <xsl:when test="rng:ref[@name='textarea']">
      <xf:textarea class="blurb" ref="{$ref}">
	<xf:label/>
      </xf:textarea>
    </xsl:when>
    <xsl:otherwise>
      <xf:input ref="{$ref}">
	<xf:label/>
      </xf:input>
    </xsl:otherwise>
  </xsl:choose>
 -->
</xsl:template>

</xsl:stylesheet>
