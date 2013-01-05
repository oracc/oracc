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
		exclude-result-prefixes="a d m rng">

<xsl:output method="xml" omit-xml-declaration="yes" indent="yes"/>

<xsl:template match="rng:grammar">
  <xsl:processing-instruction name="xml-stylesheet"
			      >href="/xsltforms/xsltforms.xsl" type="text/xsl"</xsl:processing-instruction>
  <xsl:text>&#xa;</xsl:text>
  <html
      xpd:dummy="I am the Fly">
    <head>
      <link href="/config.css" type="text/css" rel="stylesheet"/>
      <title>Oracc Configuration</title>
      <xf:model>
	<xf:instance id="config" xmlns="http://oracc.org/ns/xpd/1.0">
	  <xi:include href="default.xpd" xmlns:xi="http://www.w3.org/2001/XInclude"/>
	</xf:instance>
	<xf:instance id="template" xmlns="http://oracc.org/ns/xpd/1.0">
	  <xi:include href="default.xpd" xmlns:xi="http://www.w3.org/2001/XInclude"/>
	</xf:instance>
	<xf:instance id="display" xmlns="">
	  <data>
	    <x/>
	    <no>no</no>
	    <yes>yes</yes>
	  </data>
	</xf:instance>
	<xf:instance id="help" xmlns="">
	  <data>
	    <blob name="#empty"/>
	    <xsl:call-template name="make-blobs"/>
	  </data>
	</xf:instance>
	
	<xf:submission id="put-and-quit" method="post"
	  action="/db/oracc/manager/manager.xql?module=config-savequit">
	  <xf:toggle case="case-busy" ev:event="xforms-submit"/>
	  <xf:toggle case="case-submit-done" ev:event="xforms-submit-done"/>
	  <xf:toggle case="case-submit-error" ev:event="xforms-submit-error"/>
	</xf:submission>

	<xf:submission id="put-only" method="post"
	  action="/db/oracc/manager/manager.xql?module=config-save">
	  <xf:toggle case="case-busy" ev:event="xforms-submit"/>
	  <xf:toggle case="case-submit-done" ev:event="xforms-submit-done"/>
	  <xf:toggle case="case-submit-error" ev:event="xforms-submit-error"/>
	</xf:submission>

	<xf:submission id="cancel" method="post"
	  action="/db/oracc/manager/manager.xql?module=config-cancel">
	  <xf:toggle case="case-busy" ev:event="xforms-submit"/>
	  <xf:toggle case="case-submit-done" ev:event="xforms-submit-done"/>
	  <xf:toggle case="case-submit-error" ev:event="xforms-submit-error"/>
	</xf:submission>

	<xf:submission id="reset" method="get" replace="instance">
	  <xsl:attribute name="resource">{$configxml}</xsl:attribute>
	  <xf:toggle case="case-busy" ev:event="xforms-submit"/>
	  <xf:toggle case="case-submit-error" ev:event="xforms-submit-error"/>
	  <xf:toggle case="case-submit-done" ev:event="xforms-submit-done"/>
	</xf:submission>

	<!-- The following conditional action is fired after the form has initialized. 
	     If the default instance is empty, fill it with data from the
	     template instance. -->
	<xf:action ev:event="xforms-ready">
	  <xsl:for-each select="document('default.xpd',/)/*/*[not(self::xpd:option)]">
	    <xf:action if="not(instance('config')/xpd:{local-name()})">
	      <xf:insert nodeset="instance('config')/*"
			 origin="instance('template')/xpd:{local-name()}"
			 position="after" at="last()"/>
	      <xf:rebuild/>
	      <xf:recalculate/>
	      <xf:refresh/>
	    </xf:action>
	  </xsl:for-each>
	  <xsl:for-each select="document('default.xpd',/)/*/xpd:option">
	    <xf:action if="not(instance('config')/*[@name='{@name}'])">
	      <xf:insert nodeset="instance('config')/*"
			 origin="instance('template')/*[@name='{@name}']"
			 position="after" at="last()"/>
	      <xf:rebuild/>
	      <xf:recalculate/>
	      <xf:refresh/>
	    </xf:action>
	  </xsl:for-each>
	</xf:action>

 	<xsl:for-each select=".//rng:data[@type='boolean']">
	  <xsl:variable name="ref">
	    <xsl:call-template name="set-ref"/>
	  </xsl:variable>
	  <xf:bind nodeset="{$ref}" type="xs:boolean"/>
	</xsl:for-each>

	<xf:submission id="save" method="put" replace="none" action="file://">
	  <xf:message level="modeless" ev:event="xforms-submit-error">Submit
	  error.</xf:message>
	</xf:submission>
	
	<xf:submission id="load" method="get" serialization="none"
			   replace="instance" action="file://">
	  <xf:message level="modeless" ev:event="xforms-submit-error">Submit
	  error.</xf:message>
	</xf:submission>
	
      </xf:model>
    </head>
    <body>
      <h1>Oracc Configuration for project: 
      <xf:output value="instance('config')/xpd:n"/></h1>

      <p class="readonlies">Project type: <xf:output ref="instance('config')/xpd:type"/>; 
      public: <xf:output ref="if(instance('config')/xpd:public='false', 
      				instance('display')/no, 
				instance('display')/yes)"/></p>

      <div class="submits">
	<p>
	  <span class="savequit">
	    <xf:submit submission="save">
	      <xf:label>Save to local disk</xf:label>
	    </xf:submit>
	  </span>
	  <span class="savequit">
	    <xf:submit submission="load">
	      <xf:label>Restore from local disk</xf:label>
	    </xf:submit>
	  </span>

<!--
	  <span class="savequit">
	    <xf:submit class="store" submission="put-and-quit">
	      <xf:label>Save and Exit</xf:label>
	    </xf:submit>
	  </span>
	  <span class="savequit">
	    <xf:submit class="store" submission="put-only">
	      <xf:label>Save</xf:label>
	    </xf:submit>
	  </span>
	  <span class="cancel">
	    <xf:submit class="store" submission="reset">
	      <xf:label>Reset</xf:label>
	    </xf:submit>
	  </span>
	  <span class="cancel">
	    <xf:submit class="store" submission="cancel">
	      <xf:label>Cancel</xf:label>
	    </xf:submit>
	  </span>
 -->
	</p>
      </div>

      <div id="horiz-tab-menu">
	<xsl:apply-templates mode="tabs" select="rng:div"/>
      </div>
     
      <xf:switch>
	<xsl:apply-templates mode="case" select="rng:div"/>
       </xf:switch>

      <br/>
      <xf:switch>
	<xf:case id="ready"/>
	<xf:case id="case-busy">
	  <p>Waiting for results from server...</p>
	</xf:case>
	<xf:case id="case-submit-error">
	  <p>Submit error</p>
	  
	</xf:case>
	<xf:case id="case-submit-done">
	  <p>Submit done</p>
	</xf:case>
      </xf:switch>  
      
    </body>
  </html>
</xsl:template>

<xsl:template mode="tabs" match="rng:div">
  <xf:trigger id="tab-{@m:name}" appearance="minimal">
    <xf:label><xsl:call-template name="tab-label"/></xf:label>
    <xf:refresh/>
    <xf:action ev:event="DOMActivate">
      <xf:setvalue ref="instance('display')/x" value="instance('help')/blob[@name='#empty']"/>
      <xf:toggle case="case-{@m:name}"/>
    </xf:action>
  </xf:trigger>
</xsl:template>

<xsl:template mode="case" match="rng:div">
  <xf:case id="case-{@m:name}" appearance="minimal">
    <div id="div-{@m:name}" class="optgroup">
      <h2><xsl:call-template name="tab-label"/> &#x2014;
      <span class="groupdoc"><xsl:value-of select="a:documentation[1]"/></span>
      </h2>
      <xsl:choose>
	<xsl:when test="rng:div">
	  <xsl:for-each select="rng:div">
	    <fieldset class="{@m:name}">
	      <table>
		<xsl:apply-templates/>
	      </table>
	    </fieldset>
	  </xsl:for-each>
	</xsl:when>
	<xsl:otherwise>
	  <fieldset class="{@m:name}">
	    <table>
	      <xsl:apply-templates/>
	    </table>
	  </fieldset>
	</xsl:otherwise>
      </xsl:choose>
      <p class="help">
	<xf:output ref="instance('display')/x" incremental="all"/>
      </p>
    </div>
  </xf:case>
</xsl:template>

<xsl:template match="rng:define">
  <xsl:apply-templates/>
</xsl:template>

<xsl:template match="rng:element[@name='option']">
  <xsl:variable name="pre">
    <xsl:text>instance('config')/xpd:option[@name='</xsl:text>
  </xsl:variable>
  <xsl:variable name="post">
    <xsl:text>']/@value</xsl:text>
  </xsl:variable>
  <tr>
    <td class="option">
      <p>
	<xsl:value-of select="ancestor::rng:define/@name"/>
	<xsl:text>: </xsl:text>
      </p>
    </td>
    <td class="control">
      <xsl:variable name="ref" select="concat($pre,
				              rng:attribute[@name='name']/rng:value,
					      $post)"/>
      <p>
	<xsl:for-each select="rng:attribute[@name='value']">
	  <xsl:sort/>
	  <xsl:call-template name="xf-input">
	    <xsl:with-param name="ref" select="$ref"/>
	  </xsl:call-template>
	</xsl:for-each>
      </p>
    </td>
    <td class="doco">
      <p>
	<xsl:for-each select="rng:attribute[@name='value']">
	  <xsl:call-template name="doco"/>
	</xsl:for-each>
      </p>
    </td>
    <td class="help">
      <xsl:call-template name="help"/>
    </td>
  </tr>
</xsl:template>

<xsl:template match="rng:element[@name='n' or @name='type' or @name='public']"/>

<xsl:template match="rng:element">
  <xsl:variable name="ref">
    <xsl:text>instance('config')/xpd:</xsl:text>
    <xsl:value-of select="@name"/>
  </xsl:variable>
  <tr>
    <td class="option">
      <xsl:value-of select="ancestor::rng:define/@name"/>
      <xsl:text>: </xsl:text>
    </td>
    <xsl:choose>
      <xsl:when test="rng:ref[@name='textarea']">
	<td colspan="2" class="double">
	  <p>
	    <xsl:call-template name="xf-input">
	      <xsl:with-param name="ref" select="$ref"/>
	    </xsl:call-template>
	    <br/>
	    <xsl:call-template name="doco"/>
	  </p>
	</td>
	<td class="help">
	  <xsl:call-template name="help"/>
	</td>
      </xsl:when>
      <xsl:otherwise>
	<td class="control">
	  <xsl:call-template name="xf-input">
	    <xsl:with-param name="ref" select="$ref"/>
	  </xsl:call-template>
	</td>
	<td class="doco">
	  <xsl:call-template name="doco"/>
	</td>
	<td class="help">
	  <xsl:call-template name="help"/>
	</td>
      </xsl:otherwise>
    </xsl:choose>
  </tr>
</xsl:template>

<xsl:template match="rng:attribute">
</xsl:template>

<xsl:template match="a:*"/>

<xsl:template match="text()"/>

<xsl:template name="xf-input">
  <xsl:param name="ref"/>
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
</xsl:template>

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
  <xsl:if test="*[1][self::a:documentation]">
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
    <xf:trigger>
      <xf:label>+</xf:label>
      <xf:message ev:event="DOMActivate" level="modeless">
	<xf:output ref="instance('help')/blob[@name='{$opt-name}']"/>
      </xf:message>
<!--
      <xf:action ev:event="DOMActivate">
	<xf:setvalue ref="instance('display')/x" value="instance('help')/blob[@name='{$opt-name}']"/>
      </xf:action>
 -->
    </xf:trigger>
  </xsl:if>
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

</xsl:stylesheet>
