<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:xs="http://www.w3.org/2001/XMLSchema" 
  xmlns:esp="http://oracc.org/ns/esp/1.0" 
  xmlns:struct="http://oracc.org/ns/esp-struct/1.0" 
  xmlns:param="http://oracc.org/ns/esp-param/1.0" 
  xmlns="http://www.w3.org/1999/xhtml" 
  xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd" 
  version="2.0"
  xpath-default-namespace="http://www.w3.org/1999/xhtml">
  <xsl:include href="esp2-functions.xslt"/>
  <xsl:include href="esp2-menu.xslt"/>
  <xsl:include href="esp2-references.xslt"/>
  <xsl:include href="esp2-alphabet.xslt"/>
  <xsl:include href="esp2-site-map.xslt"/>
  <xsl:output method="xml" encoding="utf-8" indent="no"/>
  <xsl:param name="oracc"/>
  <xsl:param name="project"/>
  <xsl:param name="projesp"/>
  <xsl:param name="scripts"/>
  <xsl:param name="output-file"/>

  <xsl:variable name="esp2-global-design-width" select="800"/>

  <xsl:variable name="parameters" select="document ( concat($projesp, '/00web/00config/parameters.xml') )/param:parameters"/>
  <xsl:variable name="index-page" select="//esp:index-list[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="glossary-page" select="//esp:glossary-list[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="techterms-page" select="//esp:techterms-list[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="site-map-page" select="//esp:site-map[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="privacy-page" select="//esp:privacy-policy[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="last-modified-times" select="document ( concat($projesp, '/01tmp/last-modified-times.xml') )/esp:last-modified-times"/>
  <xsl:variable name="images-info" select="document ( concat($projesp, '/01tmp/images-info.xml') )/esp:images-info"/>

  <xsl:key name="indices" match="esp:index" use="@term"/>
  <xsl:template match="/">
<!--    <xsl:message>Phase 3: Process high-level mark-up (= mark-up giving rise to other mark-up needing processing), and add structural page components to body</xsl:message>-->
    <xsl:result-document href="{$output-file}">
      <xsl:apply-templates/>
    </xsl:result-document>
  </xsl:template>
  <!-- add structure to <body> -->
  <xsl:template match="body">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <!--<xsl:message>class=<xsl:value-of select="@class"/></xsl:message>-->
      <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>

      <div>
	<xsl:choose>
	  <xsl:when test="count(ancestor::struct:page)=1 and $parameters/param:banner">
	    <xsl:attribute name="id"><xsl:text>Banner</xsl:text></xsl:attribute>
	    <xsl:variable name="cnode" select="."/>
	    <xsl:for-each select="$parameters/param:banner">
	      <xsl:apply-templates>
		<xsl:with-param name="cnode" select="$cnode"/>
	      </xsl:apply-templates>
	    </xsl:for-each>
	  </xsl:when>
	  <xsl:otherwise>
	    <xsl:attribute name="id"><xsl:text>Header</xsl:text></xsl:attribute>
            <span id="HeadTitle">
	      <xsl:choose>
		<xsl:when test="$current-page/ancestor::struct:page[1]">
		  <esp:link page="{/struct:page/@id}" nesting="{count($current-page/ancestor::struct:page)}">
		    <xsl:copy-of select="$parameters/param:title/node()"/>
		  </esp:link>
		</xsl:when>
		<xsl:otherwise>
		  <xsl:copy-of select="$parameters/param:title/node()"/>
		</xsl:otherwise>
	      </xsl:choose>
            </span>
	    <xsl:if test="$parameters/param:subtitle">
	      <br/>
	      <span id="HeadSubtitle">
		<xsl:copy-of select="$parameters/param:subtitle/node()"/>
	      </span>
	    </xsl:if>
	  </xsl:otherwise>
	</xsl:choose>
      </div>

      <!-- main menu (screen only) -->
      <div id="Menu">
	
	<xsl:if test="$parameters/param:cuneify/@switcher='yes'">
	  <p id="switcherul">
	    <span id="CuneiformSwitcher">
	    <xsl:text>Cuneiform script: </xsl:text>
	    <span class="switcherlink" id="switcherOB">
	      <a href="#" onclick="setActiveStyleSheet('oldbabylonian'); return false;" title="Old Babylonian script">&#160;OB&#160;</a>
	    </span>
	    <span class="switcherlink" id="switcherNA">
	    <a href="#" onclick="setActiveStyleSheet('neoassyrian'); return false;" title="Neo Assyrian script">&#160;NA&#160;</a></span>
	    </span>
	  </p>
	</xsl:if>

        <xsl:if test="$parameters/param:main-menu-caption">
          <div id="MenuCaption">
            <xsl:copy-of select="$parameters/param:main-menu-caption/node()"/>
          </div>
        </xsl:if>
        <!--<xsl:when test="$current-page/ancestor::struct:page[1]">
        <div id="SelfInMenu" class="only"><xsl:value-of select="esp:name"/></div>
      </xsl:when>
      <xsl:otherwise>
        <esp:link page="{$current-page/@id}" class="{$li-class}"/>
      </xsl:otherwise>-->
        <!--<xsl:variable name="home-page-only">
        <esp:dummy><xsl:for-each select="/"><xsl:copy/></xsl:for-each></esp:dummy>
      </xsl:variable>-->
        <xsl:call-template name="menu">
          <xsl:with-param name="menu-page" select="/struct:page"/>
          <xsl:with-param name="current-page" select="$current-page" tunnel="yes"/>
          <xsl:with-param name="first-link-page" select="/struct:page"/>
        </xsl:call-template>

      </div>

      <!-- content <div> -->
      <div id="Content">
        <!-- stylesheet warning -->
        <xsl:if test="not ( $current-page/ancestor::struct:page[1] )">
          <div id="StyleSheetWarning">
            <p>
              <b>You are seeing an unstyled version of this site. If this is because you are using an older web browser, we recommend that you upgrade to a modern, standards-compliant browser such as <esp:link site-name="FireFox" url="http://www.getfirefox.com/"/>, which is available free of charge for Windows, Mac and Linux.</b>
            </p>
          </div>
        </xsl:if>

	<!-- navbar to go back/forward for signlists -->
	<xsl:if test="$parameters/param:navbar='yes'">
	  <xsl:copy-of select="div[@class='navbar']"/>
	</xsl:if>
      
        <!-- breadcrumb (screen only) -->
        <xsl:if test="count ( $current-page/ancestor::struct:page )">
          <div id="Breadcrumb">
            <xsl:for-each select="$current-page/ancestor::struct:page">
              <esp:link page="{@id}" nesting="{count($current-page/ancestor::struct:page)}"/>
              <xsl:text> » </xsl:text>
            </xsl:for-each>
            <xsl:value-of select="$current-page/esp:name"/>
          </div>
        </xsl:if>
<!-- sjt: I think this is what Ruth commented out to make room for cuneify
     font switching but I'm not sure -->
<!--        <esp:bookmark id="maincontent" title="main content" hide-highlight="yes"/>  -->
        <!-- main heading -->
        <h1>
          <xsl:value-of select="$current-page/esp:title"/>
        </h1>
        <!-- process rest of content -->
	<div id="innerContent">
          <xsl:apply-templates/>
	</div>
        <!-- references and further readings -->
        <xsl:call-template name="references"/>
        <!-- author credits and citation info -->
        <xsl:if test=".//esp:author">
          <div id="Authors">
            <p>
              <xsl:for-each select=".//esp:author">
                <xsl:value-of select="@first-names"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="@last-name"/>
                <xsl:if test="position () = last () - 1">
                  <xsl:text> &amp; </xsl:text>
                </xsl:if>
                <xsl:if test="position () &lt; last () - 1">
                  <xsl:text>, </xsl:text>
                </xsl:if>
              </xsl:for-each>
            </p>
          </div>
          <div id="CiteAs">
            <p>
              <xsl:for-each select=".//esp:author">
                <xsl:value-of select="@first-names"/>
                <xsl:text> </xsl:text>
                <xsl:value-of select="@last-name"/>
                <xsl:if test="position () = last () - 1">
                  <xsl:text> &amp; </xsl:text>
                </xsl:if>
                <xsl:if test="position () &lt; last () - 1">
                  <xsl:text>, </xsl:text>
                </xsl:if>
              </xsl:for-each>
              <xsl:text>, '</xsl:text>
              <xsl:value-of select="$current-page/esp:title"/>
              <xsl:text>', </xsl:text>
              <i>
                <xsl:value-of select="$parameters/param:title"/>
              </i>
              <xsl:text>, </xsl:text>
              <xsl:value-of select="$parameters/param:publisher"/>
              <xsl:text>, </xsl:text>
              <xsl:variable name="modified-date" select="$last-modified-times/esp:lmt[@file = $current-page/@file]"/>
              <xsl:value-of select="substring ( $modified-date, string-length ( $modified-date ) - 4 )"/>
              <xsl:text> [</xsl:text>
              <xsl:value-of select="$parameters/param:host"/>
              <xsl:value-of select="$parameters/param:root"/>
              <xsl:value-of select="$current-page/@url"/>
              <!--<span id="JSDate"><script type="text/javascript"><esp:comment>
            var aMonthNames = new Array ( 'January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December' );
            var dToday = new Date ();
            document.write ( ', accessed ' + dToday.getDate () + ' ' + aMonthNames [ dToday.getMonth () ] + ' ' + dToday.getFullYear() );
          // </esp:comment></script></span>-->
              <!--<xsl:text>, accessed </xsl:text>-->
              <esp:comment>#config timefmt=", accessed %d %B %Y" </esp:comment>
              <esp:comment>#echo var="DATE_LOCAL" </esp:comment>
              <xsl:text>]</xsl:text>
            </p>
          </div>
        </xsl:if>
        <!-- clearing spacer -->
        <div id="EndContentSpace"> </div>
      </div>
      <!-- back to top link (screen only) -->
      <div id="BackToTop">
        <esp:link bookmark="top" title="">Back to top ^^</esp:link>
      </div>
      <!-- footers (including right footer for screen only) -->
      <div id="FooterWhole"> </div>
      <div id="FooterRight">
        <xsl:for-each select="//struct:page[@show-small-print-link = 'yes']">
          <xsl:if test="position () != 1"> / </xsl:if>
          <xsl:choose>
            <xsl:when test="@id = $current-page/@id">
              <xsl:value-of select="esp:name"/>
            </xsl:when>
            <xsl:otherwise>
              <esp:link page="{@id}" nesting="{count($current-page/ancestor::struct:page)}"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
      </div>
      <div id="FooterLeft">
        <!--<xsl:copy-of select="$parameters/param:footer/node ()"/>-->
	<xsl:variable name="cnode" select="."/>
	<xsl:for-each select="$parameters/param:footer">
	  <xsl:apply-templates>
	    <xsl:with-param name="cnode" select="$cnode"/>
	  </xsl:apply-templates>
	</xsl:for-each>
      </div>
      <!-- site tabs-->
      <xsl:variable name="context-page" select="."/>
      <!-- address <div> -->
      <div id="URL">
        <xsl:value-of select="$parameters/param:host"/>
        <xsl:value-of select="$parameters/param:root"/>
        <xsl:value-of select="$current-page/@url"/>
      </div>
      <!-- google analytics script: account number should be UA-602782-2 for enlil dev site and UA-602782-3 for PRS servers at Leeds
  <script src="http://www.google-analytics.com/urchin.js" type="text/javascript">
  </script>
  <script type="text/javascript">
  _uacct = "UA-602782-3";
  urchinTracker();
  </script>-->
    </xsl:copy>
  </xsl:template>

  <!-- process images -->
  <xsl:template match="esp:image">
    <xsl:param name="cnode" select="."/>
    <xsl:variable name="relpath">
      <xsl:call-template name="set-relpath">
	<xsl:with-param name="context-node" select="$cnode"/>
      </xsl:call-template>
    </xsl:variable>
    <xsl:if test="not ( @description )">
      <xsl:message><xsl:value-of select="ancestor::struct:page[1]/@file"/>: image tag lacks description (name: <xsl:value-of select="@name"/>) -- page will fail WAI compliance test</xsl:message>
    </xsl:if>
    <xsl:variable name="src" select="if ( @file ) then concat ( $relpath, '/images/', @file ) else @url"/>

<!-- SJT: compute grid from images-info and esp2-global-design-width -->
<!--    <xsl:variable name="width" select="if ( @file ) then @grid else @width"/>  -->

   <xsl:variable name="this-file" select="@file"/>
   <xsl:variable name="img-width" select="$images-info/esp:image-info[@file = $this-file]/@width"/>
<!--   <xsl:message>found width <xsl:value-of select="$img-width"/> for file <xsl:value-of select="$this-file"/></xsl:message> -->
   <xsl:variable name="width">
     <xsl:choose>
       <xsl:when test="@file">
	 <xsl:choose>
	   <xsl:when test="$img-width >= $esp2-global-design-width">
	     <xsl:value-of select="100"/>
	   </xsl:when>
	   <xsl:otherwise>
	     <xsl:variable name="pc" select="5 + floor(100 * ($img-width div $esp2-global-design-width))"/>
	     <xsl:value-of select="floor($pc div 5) * 5"/>
	   </xsl:otherwise>
	 </xsl:choose>
       </xsl:when>
<!-- No: but perhaps we need a way to specify a percentage width for non-local files ?
       <xsl:otherwise>
	 <xsl:value-of select="@width"/>
       </xsl:otherwise>
 -->
     </xsl:choose>
   </xsl:variable>
   
<!--   <xsl:message>grid fit for <xsl:value-of select="@file"/> set from <xsl:value-of select="$img-width"/> to <xsl:value-of select="$width"/></xsl:message> -->

   <xsl:variable name="div-or-span">
     <xsl:choose>
       <!-- add xsl:when clauses here to identify additional contexts
            where an image should be text-inlined -->
       <xsl:when test="ancestor::p"><xsl:text>span</xsl:text></xsl:when>
       <xsl:when test="ancestor::dt"><xsl:text>span</xsl:text></xsl:when>
       <xsl:otherwise><xsl:text>div</xsl:text>
       </xsl:otherwise>
     </xsl:choose>
   </xsl:variable>

   <xsl:element name="{$div-or-span}">
     <xsl:attribute name="class">
       <xsl:choose>
	 <xsl:when test="@position='float'">
	   <xsl:value-of select="concat('imagefloat ', concat('pc',$width))"/>
	 </xsl:when>
	 <xsl:otherwise>
	   <xsl:text>imageinline</xsl:text>
	 </xsl:otherwise>
       </xsl:choose>
     </xsl:attribute>
     
     <xsl:for-each select="esp:link">
       <xsl:copy>
         <xsl:copy-of select="@*"/>
         <img src="{$src}" alt="{@description}"/>
       </xsl:copy>
     </xsl:for-each>
     <xsl:if test="not(esp:link)">
       <img src="{$src}" alt="{@description}">
	 <xsl:copy-of select="@class|@id"/>
	 <xsl:if test="@height">
	   <xsl:attribute name="height"><xsl:value-of select="@height"/></xsl:attribute>
	 </xsl:if>
	 <xsl:if test="@width">
	   <xsl:attribute name="width"><xsl:value-of select="@width"/></xsl:attribute>
	 </xsl:if>
         <xsl:if test="esp:image-map">
           <xsl:attribute name="usemap" select="concat('#map_', generate-id(esp:image-map))"/>
         </xsl:if>
       </img>
     </xsl:if>
     <xsl:for-each select="esp:caption">
       <div class="imagecaption">
         <p>
	   <xsl:apply-templates/> <!-- check if this starts with p tag -->
	 </p>
       </div>
     </xsl:for-each>
     <xsl:for-each select="esp:image-map">
       <map id="map_{generate-id()}" name="map_{generate-id()}">
         <xsl:apply-templates/>
       </map>
     </xsl:for-each>
   </xsl:element>
  </xsl:template>

  <!-- process headings -->
  <xsl:template match="esp:h | esp:sh">
    <xsl:variable name="heading-text">
      <xsl:apply-templates mode="dummy"/>
    </xsl:variable>
    <esp:bookmark id="h_{esp:make-alphanumeric ( $heading-text )}" hide-highlight="yes"/>
    <xsl:element name="{if ( self::esp:h ) then 'h2' else 'h3'}">
      <xsl:apply-templates/>
    </xsl:element>
  </xsl:template>
  <!-- process site map -->
  <xsl:template match="esp:site-map">
    <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
    <div id="SiteMap">
      <xsl:call-template name="site-map">
        <xsl:with-param name="map-page" select="/struct:page"/>
        <xsl:with-param name="current-page" select="$current-page" tunnel="yes"/>
      </xsl:call-template>
    </div>
  </xsl:template>
  <!-- process reference links -->
  <xsl:template match="esp:ref">
    <xsl:variable name="current-body" select="ancestor::body[1]"/>
    <xsl:variable name="reference-id" select="generate-id ()"/>
    <xsl:for-each select="$current-body//esp:ref">
      <xsl:if test="generate-id () = $reference-id">
        <esp:bookmark id="reflink_{position ()}" hide-highlight="yes"/>
        <span id="highlight_reflink_{position()}">
          <xsl:text>(</xsl:text>
          <esp:link bookmark="ref_{position ()}" title="Jump to reference, below">
            <xsl:value-of select="position ()"/>
          </esp:link>
          <xsl:text>)</xsl:text>
        </span>
      </xsl:if>
    </xsl:for-each>
  </xsl:template>
  <!-- process (strip) further reading elements -->
  <xsl:template match="esp:reading"/>
  <!-- glossary links -->
  <xsl:template match="esp:glossary">
    <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
    <xsl:if test="not ( $glossary-page//esp:glossary-list/esp:definition[@term = current ()/@term] )">
      <xsl:message>  WARNING! Undefined glossary term '<xsl:value-of select="@term"/>' on page with id '<xsl:value-of select="$current-page/@id"/>'</xsl:message>
    </xsl:if>
    <span class="glossaryterm">
      <xsl:apply-templates/>
    </span>
    <span class="glossarylink">
      <esp:link page="{$glossary-page/@id}" bookmark="{esp:make-alphanumeric ( @term )}" title="Go to '{@term}' in {$glossary-page/esp:name}" hide-print="yes">
        <xsl:text> PGP </xsl:text>
      </esp:link>
    </span>
  </xsl:template>
  <!-- process glossary page -->
  <xsl:template match="esp:glossary-list">
    <xsl:variable name="usedletters" as="xs:string *">
      <xsl:for-each-group select="esp:definition" group-by="translate( upper-case ( substring ( @term , 1, 1 ) ), '[', '~')">
        <xsl:value-of select="current-grouping-key ()"/>
      </xsl:for-each-group>
    </xsl:variable>
    <xsl:call-template name="alphabet">
      <xsl:with-param name="usedletters" select="$usedletters"/>
    </xsl:call-template>
    <div id="Glossary">
      <xsl:for-each-group select="esp:definition" group-by="translate( upper-case ( substring ( @term , 1, 1 ) ), '[', '~')">
        <xsl:sort select="current-grouping-key ()" 
                  collation="http://saxon.sf.net/collation?ignore-modifiers=yes"/>
	<div class="letter">
          <esp:bookmark id="letter_{translate(current-grouping-key (), '~', '_')}" hide-highlight="yes"/>
          <h2>
	    <xsl:choose>
	      <xsl:when test="current-grouping-key() = '~'">
		<xsl:text>[X]</xsl:text>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="current-grouping-key ()"/>
	      </xsl:otherwise>
	    </xsl:choose>
          </h2>
          <xsl:for-each select="current-group ()">
            <xsl:sort select="@term" 
                      collation="http://saxon.sf.net/collation?ignore-modifiers=yes"
		      />
            <dl id="highlight_{esp:make-alphanumeric ( @term )}">
              <dt>
		<esp:bookmark id="{esp:make-alphanumeric ( @term )}" hide-highlight="yes"/>
		<xsl:value-of select="@term"/>
              </dt>
              <dd>
		<xsl:apply-templates/>
              </dd>
            </dl>
          </xsl:for-each>
	</div>
      </xsl:for-each-group>
    </div>
  </xsl:template>
  <!-- techterms links -->
  <xsl:template match="esp:techterms">
    <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
    <xsl:if test="not ( $techterms-page//esp:techterms-list/esp:termsdefinition[@term = current ()/@term] )">
      <xsl:message>  WARNING! Undefined technical term '<xsl:value-of select="@term"/>' on page with id '<xsl:value-of select="$current-page/@id"/>'</xsl:message>
    </xsl:if>
    <span class="techterms">
      <xsl:apply-templates/>
    </span>
    <span class="techtermslink">
      <esp:link page="{$techterms-page/@id}" bookmark="{esp:make-alphanumeric ( @term )}" title="Go to '{@term}' in {$techterms-page/esp:name}" hide-print="yes">
        <xsl:text> TT </xsl:text>
      </esp:link>
    </span>
  </xsl:template>
  <!-- process techterms page -->
  <xsl:template match="esp:techterms-list">
    <xsl:variable name="usedletters" as="xs:string *">
      <xsl:for-each-group select="esp:termsdefinition" group-by="translate( upper-case ( substring ( @term , 1, 1 ) ), '[', '~')">
        <xsl:value-of select="current-grouping-key ()"/>
      </xsl:for-each-group>
    </xsl:variable>
    <xsl:call-template name="alphabet">
      <xsl:with-param name="usedletters" select="$usedletters"/>
    </xsl:call-template>
    <div id="Techterms">
      <xsl:for-each-group select="esp:termsdefinition" group-by="translate( upper-case ( substring ( @term , 1, 1 ) ), '[', '~')">
        <xsl:sort select="current-grouping-key ()" 
                  collation="http://saxon.sf.net/collation?ignore-modifiers=yes"/>
	<div class="letter">
          <esp:bookmark id="letter_{current-grouping-key ()}" hide-highlight="yes"/>
          <h2>
	    <xsl:choose>
	      <xsl:when test="current-grouping-key() = '~'">
		<xsl:text>[X]</xsl:text>
	      </xsl:when>
	      <xsl:otherwise>
		<xsl:value-of select="current-grouping-key ()"/>
	      </xsl:otherwise>
	    </xsl:choose>
          </h2>
          <xsl:for-each select="current-group ()">
            <xsl:sort select="@term" 
                      collation="http://saxon.sf.net/collation?ignore-modifiers=yes"/>
            <dl id="highlight_{esp:make-alphanumeric ( @term )}">
              <dt>
		<esp:bookmark id="{esp:make-alphanumeric ( @term )}" hide-highlight="yes"/>
		<xsl:value-of select="@term"/>
              </dt>
              <dd>
		<xsl:apply-templates/>
              </dd>
            </dl>
          </xsl:for-each>
	</div>
      </xsl:for-each-group>
    </div>
  </xsl:template>

  <!-- process link sets -->
  <xsl:template match="struct:page-links">
    <span id="PageLinks">
      <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
      <xsl:variable name="pages" select="if ( @relation = 'child' ) then $current-page/struct:page[not (@hide-menu-link = 'yes')] else $current-page/ancestor::struct:page[1]/struct:page[not (@hide-menu-link = 'yes')]"/>
      <xsl:for-each select="$pages">
        <xsl:if test="position () != 1"> / </xsl:if>
        <xsl:choose>
          <xsl:when test="@id = $current-page/@id">
            <xsl:value-of select="esp:name"/>
          </xsl:when>
          <xsl:otherwise>
            <esp:link page="{@id}" nesting="{count($current-page/ancestor::struct:page)}"/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>
    </span>
  </xsl:template>
  <!-- remove author tags -->
  <xsl:template match="esp:author"/>
  <!-- email addresses -->
  <xsl:template match="esp:email">
    <xsl:variable name="replaced-address" select="replace(replace(@address,'\.',' dot '),'@',' at ')"/>
    <span class="obfuscatedEmailAddress">
      <xsl:value-of select="$replaced-address"/>
    </span>
    <!--<xsl:variable name="reversed-address" select="codepoints-to-string(reverse(string-to-codepoints($replaced-address)))"/>
  <span class="obfuscatedEmailAddress"><xsl:value-of select="$reversed-address"/></span>
  <span class="obfuscatedEmailExplanation"> (because JavaScript is not available in your browser, you are seeing this email address in a form altered to prevent spamming: you will need to read it backwards and make appropriate substitutions)</span>-->
  </xsl:template>

  <xsl:template match="div[@class='navbar']"/>
  
  <!-- copy the rest unchanged -->
  <xsl:template match="*">
    <xsl:param name="cnode" select="."/>
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates>
	<xsl:with-param name="cnode" select="$cnode"/>
      </xsl:apply-templates>
    </xsl:copy>
  </xsl:template>

<xsl:template name="tab">
  <xsl:message>The 'tab' feature is no longer part of ESP; please revise your portal pages appropriately.</xsl:message>
</xsl:template>

</xsl:stylesheet>
