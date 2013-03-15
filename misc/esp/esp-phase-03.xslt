<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet 
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:xs="http://www.w3.org/2001/XMLSchema" 
  xmlns:esp="http://oracc.org/ns/esp/1.0" 
  xmlns:app="http://oracc.org/ns/esp-appearance/1.0" 
  xmlns:struct="http://oracc.org/ns/esp-struct/1.0" 
  xmlns:param="http://oracc.org/ns/esp-param/1.0" 
  xmlns="http://www.w3.org/1999/xhtml" 
  xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd" 
  version="2.0"
  xpath-default-namespace="http://www.w3.org/1999/xhtml"
>
  <xsl:include href="esp-functions.xslt"/>
  <xsl:include href="esp-menu.xslt"/>
  <xsl:include href="esp-references.xslt"/>
  <xsl:include href="esp-alphabet.xslt"/>
  <xsl:include href="esp-site-map.xslt"/>
  <xsl:output method="xml" encoding="utf-8" indent="no"/>
  <xsl:param name="oracc" select="'/usr/local/oracc/'"/>
  <xsl:param name="project"/>
  <xsl:param name="projesp" select="concat($oracc, $project, '/00web/esp')"/>
  <xsl:param name="scripts" select="concat($oracc,'lib/scripts')"/>
  <xsl:param name="output-file"/>
  <xsl:variable name="parameters" select="document ( concat($projesp, '/site-content/parameters.xml') )/param:parameters"/>
  <xsl:variable name="index-page" select="//esp:index-list[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="glossary-page" select="//esp:glossary-list[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="techterms-page" select="//esp:techterms-list[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="site-map-page" select="//esp:site-map[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="privacy-page" select="//esp:privacy-policy[1]/ancestor::struct:page[1]"/>
  <xsl:variable name="last-modified-times" select="document ( concat($projesp, '/temporary-files/last-modified-times.xml') )/esp:last-modified-times"/>
  <xsl:variable name="images-info" select="document ( concat($projesp, '/temporary-files/images-info.xml') )/esp:images-info"/>
  <xsl:variable name="appearance" 
		select="document(concat($projesp,'/site-content/appearance.xml'))/app:appearance"/>

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
      <xsl:variable name="current-page" select="ancestor::struct:page[1]"/>
      <!-- access links (screen only) -->
      <div id="AccessLinks">
        <!-- top bookmark -->
        <esp:bookmark id="top" hide-highlight="yes"/>
<!--        <esp:link bookmark="maincontent" title="" accesskey="s">Skip to main content</esp:link> -->
        <xsl:variable name="show-page-first" select="//struct:page[@show-access-link = 'yes'][1]"/>
        <xsl:for-each select="//struct:page[@show-access-link = 'yes']">
	  <xsl:if test="not(.=$show-page-first)">
	    <xsl:text> / </xsl:text>
	  </xsl:if>
          <xsl:choose>
            <xsl:when test="@id = $current-page/@id">
              <xsl:value-of select="esp:name"/>
            </xsl:when>
            <xsl:otherwise>
              <esp:link page="{@id}"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
        <span id="TextSize">
          <xsl:text> / Text size: </xsl:text>
          <span class="textsizelink" id="TextSizeSmaller">
            <a href="#" onclick="if ( window.changeTextSize ) changeTextSize ( -1 ); return false;" title="Smaller text" accesskey="8"> - </a>
          </span>
          <span class="textsizelink" id="TextSizeLarger">
            <a href="#" onclick="if ( window.changeTextSize ) changeTextSize ( 1 ); return false;" title="Larger text" accesskey="9"> + </a>
          </span>
        </span>

	<xsl:if test="$parameters/param:cuneify/@switcher='yes'">
	  <span id="CuneiformSwitcher">
	    <xsl:text> / Cuneiform script: </xsl:text>
	    <span class="switcherlink" id="switcherOB">
	      <a href="#" onclick="setActiveStyleSheet('oldbabylonian'); return false;" title="Old Babylonian script">&#160;OB&#160;</a>
	    </span>
	    <span class="switcherlink" id="switcherNA">
	    <a href="#" onclick="setActiveStyleSheet('neoassyrian'); return false;" title="Neo Assyrian script">&#160;NA&#160;</a></span>
	  </span>
	</xsl:if>

      </div>
      <!-- reference links (screen only) -->
      <div id="ReferenceLinks">
        <xsl:for-each select="//struct:page[@show-reference-link = 'yes']">
          <xsl:if test="position () != 1"> / </xsl:if>
          <xsl:choose>
            <xsl:when test="@id = $current-page/@id">
              <xsl:value-of select="esp:name"/>
            </xsl:when>
            <xsl:otherwise>
              <esp:link page="{@id}"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
      </div>
      <!-- search box (screen only) 
      Google CSE Search Box Begins 
      <form id="searchbox_005494896642034120825:x2xmpbt2cbm" action="">
       <input type="hidden" name="cx" value="005494896642034120825:x2xmpbt2cbm" />
       <input type="hidden" name="cof" value="FORID:9" />
       <input name="q" type="text" size="40" />
       <input type="submit" name="sa" value="Search" />
       <img src="http://google.com/coop/images/google_custom_search_smnar.gif" alt="Google Custom Search" />
      </form>
       Google CSE Search Box Ends -->
      <!--bastardised CSE ver 1:
      
      <form id="searchbox_005494896642034120825:x2xmpbt2cbm" action="">
          <div id="Search">
          <xsl:text>&#160;&#160;</xsl:text>
          <label for="q" accesskey="4">Search</label>
          <xsl:text>&#160;</xsl:text>
          <input type="hidden" name="cx" value="005494896642034120825:x2xmpbt2cbm" />
          <input type="hidden" name="cof" value="FORID:9" />
          <input name="q" type="text" size="20" />
          <input type="submit" name="sa" value="Search" />
          <img src="http://google.com/coop/images/google_custom_search_smnar.gif" alt="Google Custom Search" />
        </div>
      </form>-->
      <!--OLD GOOGLE PSS CODE USED ON WH SITE: KEEP FOR NOW FOR COMPARISON. DELETE WHEN DONE
    <form action="http://www.google.com/u/whipplemuseum" method="get">
      <div id="Search">
        <xsl:text>&#160;&#160;</xsl:text>
        <label for="q" accesskey="4">Search</label>
        <xsl:text>&#160;</xsl:text>
        <input type="text" id="q" name="q" size="12" value="" maxlength="255"/>
        <input type="hidden" name="hq" value="inurl:www.hps.cam.ac.uk/whipple/explore"/>
        <xsl:text>&#160;</xsl:text>
        <input type="submit" value="Search"/>
      </div>
    </form>-->
      <!-- header -->
      <div id="Header">
        <span id="HeadTitle">
	  <xsl:choose>
	    <xsl:when test="$current-page/ancestor::struct:page[1]">
	      <esp:link page="{/struct:page/@id}">
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

      </div>
      <!-- main menu (screen only) -->
      <div id="Menu">
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
        <!-- breadcrumb (screen only) -->
        <xsl:if test="count ( $current-page/ancestor::struct:page )">
          <div id="Breadcrumb">
            <xsl:for-each select="$current-page/ancestor::struct:page">
              <esp:link page="{@id}"/>
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
        <xsl:apply-templates/>
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
              <esp:link page="{@id}"/>
            </xsl:otherwise>
          </xsl:choose>
        </xsl:for-each>
      </div>
      <div id="FooterLeft">
        <xsl:copy-of select="$parameters/param:footer/node ()"/>
      </div>
      <!-- site tabs-->
      <xsl:for-each select="$appearance/app:tabs/*">
	<xsl:call-template name="tab"/>
      </xsl:for-each>
<!--
      <div id="tab_cams">
        <esp:link url="http://cdl.museum.upenn.edu/cams/">
          <img src="{$parameters/param:root}/images/tab_cams.gif" alt="Corpus of Ancient Mesopotamian Scholarship" width="28" height="117"/>
        </esp:link>
      </div>
      <div id="tab_knp">
        <esp:link url="http://knp.prs.heacademy.ac.uk">
          <img src="{$parameters/param:root}/images/tab_knp.gif" alt="Knowledge and Power" width="28" height="117"/>
        </esp:link>
      </div>
      <div id="tab_saa">
        <esp:link url="http://cdl.museum.upenn.edu/saa/">
          <img src="{$parameters/param:root}/images/tab_saa.gif" alt="State Archives of Assyria" width="28" height="117"/>
        </esp:link>
      </div>
 -->
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
    <xsl:if test="not ( @description )">
      <xsl:message>  WARNING! Image tag lacking description (name: <xsl:value-of select="@name"/>) -- page will fail WAI compliance test!</xsl:message>
    </xsl:if>
    <xsl:variable name="src" select="if ( @file ) then concat ( $parameters/param:root, '/images/', @file ) else @url"/>
    <xsl:variable name="width" select="if ( @file ) then $images-info/esp:image-info/@width[../@file = current ()/@file] else @width"/>
    <xsl:variable name="height" select="if ( @file ) then $images-info/esp:image-info/@height[../@file = current ()/@file] else @height"/>
    <div>
      <xsl:attribute name="class" select="if ( @position = 'float' ) then 'imagefloat' else 'imageinline'"/>
      <xsl:if test="$width">
        <xsl:attribute name="style">width: <xsl:value-of select="$width"/>px</xsl:attribute>
      </xsl:if>
      <xsl:for-each select="esp:link | esp:flash-movie">
        <xsl:copy>
          <xsl:copy-of select="@*"/>
          <img src="{$src}" alt="{@description}">
            <xsl:if test="$width">
              <xsl:attribute name="width" select="$width"/>
            </xsl:if>
            <xsl:if test="$height">
              <xsl:attribute name="height" select="$height"/>
            </xsl:if>
          </img>
        </xsl:copy>
      </xsl:for-each>
      <xsl:if test="not(esp:link | esp:flash-movie)">
        <img src="{$src}" alt="{@description}">
          <xsl:if test="$width">
            <xsl:attribute name="width" select="$width"/>
          </xsl:if>
          <xsl:if test="$height">
            <xsl:attribute name="height" select="$height"/>
          </xsl:if>
          <xsl:if test="esp:image-map">
            <xsl:attribute name="usemap" select="concat('#map_', generate-id(esp:image-map))"/>
          </xsl:if>
        </img>
      </xsl:if>
      <xsl:for-each select="esp:caption">
        <div class="imagecaption">
          <xsl:apply-templates/>
        </div>
      </xsl:for-each>
      <xsl:for-each select="esp:image-map">
        <map id="map_{generate-id()}" name="map_{generate-id()}">
          <xsl:apply-templates/>
        </map>
      </xsl:for-each>
    </div>
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
      <xsl:for-each-group select="esp:definition" group-by="upper-case ( substring ( @term , 1, 1 ) )">
        <xsl:value-of select="current-grouping-key ()"/>
      </xsl:for-each-group>
    </xsl:variable>
    <xsl:call-template name="alphabet">
      <xsl:with-param name="usedletters" select="$usedletters"/>
    </xsl:call-template>
    <div id="Glossary">
      <xsl:for-each-group select="esp:definition" group-by="upper-case ( substring ( @term , 1, 1 ) )">
        <xsl:sort select="current-grouping-key ()" 
                  collation="http://saxon.sf.net/collation?ignore-modifiers=yes"
/>
        <esp:bookmark id="letter_{current-grouping-key ()}" hide-highlight="yes"/>
        <h2>
          <xsl:value-of select="current-grouping-key ()"/>
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
      <xsl:for-each-group select="esp:termsdefinition" group-by="upper-case ( substring ( @term , 1, 1 ) )">
        <xsl:value-of select="current-grouping-key ()"/>
      </xsl:for-each-group>
    </xsl:variable>
    <xsl:call-template name="alphabet">
      <xsl:with-param name="usedletters" select="$usedletters"/>
    </xsl:call-template>
    <div id="Techterms">
      <xsl:for-each-group select="esp:termsdefinition" group-by="upper-case ( substring ( @term , 1, 1 ) )">
        <xsl:sort select="current-grouping-key ()" 
                  collation="http://saxon.sf.net/collation?ignore-modifiers=yes"
/>
        <esp:bookmark id="letter_{current-grouping-key ()}" hide-highlight="yes"/>
        <h2>
          <xsl:value-of select="current-grouping-key ()"/>
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
            <esp:link page="{@id}"/>
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
  <!-- copy the rest unchanged -->
  <xsl:template match="*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

<xsl:template name="tab">
<!--  <xsl:message>processing tab <xsl:value-of select="app:alt"/></xsl:message> -->
  <div id="tab_{position()}">
    <esp:link url="{app:url}" title="{app:text}">
      <img src="{$parameters/param:root}/images/{app:img}" alt="{app:alt}" width="{app:img/@width}" height="{app:img/@height}"/>
    </esp:link>
  </div>
</xsl:template>
</xsl:stylesheet>
