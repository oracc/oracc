<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet
	xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
	xsi:schemaLocation="http://www.w3.org/1999/XSL/Transform http://www.w3.org/2005/02/schema-for-xslt20.xsd"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
	xmlns:esp="http://oracc.org/ns/esp/1.0"
	xmlns:param="http://oracc.org/ns/esp-param/1.0"
	xmlns:app="http://oracc.org/ns/esp-appearance/1.0"
	xmlns="http://www.w3.org/1999/xhtml"
	version="2.0"
	xpath-default-namespace="http://www.w3.org/1999/xhtml"
>
<xsl:output
	name="css"
	method="text"
	encoding="utf-8"
	include-content-type="no"
	indent="no"
	media-type="text/css"
/>
<xsl:param name="oracc" select="'/usr/local/oracc/'"/>
<xsl:param name="project"/>
<xsl:param name="projesp" select="concat($oracc, $project, '/00web/esp')"/>
<xsl:param name="scripts" select="concat($oracc,'lib/scripts')"/>

<xsl:param name="output-directory"/>
<xsl:variable name="parameters" 
  select="document ( concat($projesp,'/site-content/parameters.xml') )/param:parameters"/>
<xsl:variable name="media" as="xs:string *">
	<xsl:text>screen</xsl:text>
	<xsl:text>print</xsl:text>
	<xsl:text>handheld</xsl:text>
	<xsl:text>browsersthatsuck</xsl:text>
</xsl:variable>
<xsl:variable name="newline">
<xsl:text>
</xsl:text>
</xsl:variable>

<xsl:variable name="appearance" 
              select="document(concat($projesp,'/site-content/appearance.xml'))/app:appearance"/>

<xsl:template match="/">
  <xsl:message>
  Creating CSS files (for these media: <xsl:for-each select="$media"><xsl:if test="position () != 1">, </xsl:if><xsl:value-of select="."/></xsl:for-each>)</xsl:message>
  <xsl:for-each select="$media">
    <xsl:result-document href="{$output-directory}/css/{.}.css" format="css">
      <xsl:variable name="css-text">
	<xsl:call-template name="css-rules">
	  <xsl:with-param name="medium" select="."/>
	</xsl:call-template>
      </xsl:variable>
      <!-- remove empty rules -->
      <xsl:variable name="css-text-no-empties">
	<xsl:analyze-string select="normalize-space ( $css-text )" regex="[^}}{{;]+ \{{ \}}">
	  <xsl:non-matching-substring><xsl:value-of select="."/></xsl:non-matching-substring>
	</xsl:analyze-string>
      </xsl:variable>
      <!-- beautify output with newlines -->
      <xsl:analyze-string select="$css-text-no-empties" regex="(;|\}}|\{{|\*/) ">
	<xsl:matching-substring><xsl:value-of select="regex-group ( 1 )"/><xsl:value-of select="$newline"/></xsl:matching-substring>
	<xsl:non-matching-substring><xsl:value-of select="."/></xsl:non-matching-substring>
      </xsl:analyze-string>
    </xsl:result-document>
  </xsl:for-each>
</xsl:template>

<xsl:template name="css-rules">
	<xsl:param name="medium"/>
html {
	font-size: 0.9em;
}
body {
	<xsl:if test="contains ( 'screen, print', $medium )">
		margin: 0;
		padding: 0.2em 0 1em 0;
		font-size: 1em;
	</xsl:if>
	<xsl:if test="contains ( 'screen', $medium )">
		font-family: <xsl:value-of select="$appearance/app:fonts/app:body-screen"/>;
		line-height: 1.6em;
	</xsl:if>
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		color: <xsl:value-of select="$appearance/app:colours/app:text"/>;
		background: <xsl:value-of select="$appearance/app:colours/app:page"/>;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		font-family: <xsl:value-of select="$appearance/app:fonts/app:body-print"/>;
		color: #000000;
		background: #ffffff;
	</xsl:if>
}
ol {
	list-style-type: decimal;
}
ol ol {
	list-style-type: lower-alpha;
}
ol ol ol {
	list-style-type: lower-roman;
}
ol ol ol ol {
	list-style-type: upper-alpha;
}
<xsl:if test="contains ( 'browsersthatsuck', $medium )">
	/* This stylesheet is targeted at old browsers that support the &lt;link rel="stylesheet" ...&gt; tag, but do not understand the media="xyz" attribute (I'm looking at you, IE 4.01 Mac), with the result that they process ALL stylesheets in all circumstances, and thus hide the menu etc. on the screen, making the site unusable. It unhides anything hidden by the print or handheld stylesheets that we'd rather have visible on the screen. */
</xsl:if>
<xsl:if test="contains ( 'print, screen', $medium )">
	p.firstpara {
		font-weight: bold;
	}
</xsl:if>
<xsl:if test="contains ( 'print, screen', $medium )">
	p {
		margin: 0 0 1.3em;
	}
	blockquote {
		margin: 0 2em 1.3em;
		font-size: 0.9em;
	}
	input {
		font-size: inherit;
		<!-- helps IE/Mac -->
	}
	div.imagefloat {
		margin: 0 0 0.5em 1em;
		float: right;
		clear: right;
		page-break-inside: avoid;
		<xsl:if test="contains ( 'handheld', $medium )">
			max-width: 100%;
		</xsl:if>
	}
	div.imageinline {
		margin: 1em 0;
		page-break-inside: avoid;
		<xsl:if test="contains ( 'handheld', $medium )">
			max-width: 100%;
		</xsl:if>
	}
	div.imageinline img, div.imagefloat img {
		display: block;
	}

	<!-- For prettier tables. RH 9.7.2009 margins default: 1em 1em 1em 2em;-->
	
	table.pretty {
		  margin: 2em 2em 2em 3em;
		  background: #f5f5f5;
		  border-collapse: collapse;
		}
		table.pretty th, table.pretty td {
		  border: 1px #c0c0c0 solid;
		  padding: 0.2em;
		}
		table.pretty th {
		  background: #dcdcdc;
		  text-align: left;
		}
		table.pretty caption {
		  margin-left: inherit;
		  margin-right: inherit;
		}

</xsl:if>
<xsl:if test="contains ( 'screen, print, handheld', $medium )">
	img {
		border-width: 0;
		<xsl:if test="contains ( 'handheld', $medium )">
			max-width: 100%;
		</xsl:if>
	}
	form {
		margin: 0;
	}
	div.imagecaption {
		margin: 0.5em 0 1.25em;
		font-size: 0.9em;
		line-height: normal;
		<xsl:if test="contains ( 'handheld', $medium )">
			max-width: 100%;
		</xsl:if>
	}
</xsl:if>
<xsl:if test="contains ( 'handheld', $medium )">
	input {
		max-width: 100%;
	}
</xsl:if>
<!-- headings -->
<xsl:if test="contains ( 'screen, print', $medium )">
h1 {
	font-family: <xsl:value-of select="$appearance/app:fonts/app:h1"/>;
	margin: 1.5em 0 1.3em;
	font-size: 1.4em;
	line-height: 1.3em;
}
h2 {
	font-family: <xsl:value-of select="$appearance/app:fonts/app:h2"/>;
	margin: 1.5em 0 1.3em;
	font-size: 1.4em;
	line-height: 1.3em;
}
h3 {
	margin: 1.5em 0 1.3em;
	font-size: 1.1em;
	line-height: 1.4em;
}
li h2, li h3 {
	<!-- for Safari -->
	margin-top: 0;
}
</xsl:if>
<xsl:if test="contains ( 'screen, handheld', $medium )">
	span.externallinktext {
		display: none;
	}
</xsl:if>
<xsl:if test="contains ( 'screen, handheld', $medium )">
span.glossarylink, span.techtermslink, span.indexlink, span.textsizelink, span.switcherlink {
		border: 1px solid <xsl:value-of select="$appearance/app:colours/app:outline"/>;
		background: <xsl:value-of select="$appearance/app:colours/app:page"/>;
}
span.glossarylink a, span.techtermslink a, span.indexlink a, span.textsizelink a, span.switcherlink a {
	text-decoration: none;
}
</xsl:if>
span.glossarylink, span.techtermslink, span.indexlink, span.switcherlink<xsl:if test="contains ( 'screen', $medium )">, span.textsizelink, </xsl:if> {
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: inline;
	</xsl:if>
}
<xsl:if test="contains ( 'screen, handheld', $medium )">
	span.glossarylink, span.techtermslink, span.indexlink {
		margin-left: 0.3em;
		font-size: 0.7em;
		vertical-align: top;
	}
</xsl:if>
<xsl:if test="contains ( 'screen, handheld', $medium )">
	span.glossaryterm, span.techterms, span.indexterm  {
		border-bottom: 1px solid <xsl:value-of select="$appearance/app:colours/app:outline"/>;
	}
	.highlighted {
		background: <xsl:value-of select="$appearance/app:colours/app:highlight"/>;
	}
</xsl:if>
<!-- links (we specify transparent backgrounds where this shouldn't be necessary, to deal with strange behaviour on the part of Opera 5 and IE5 - note also that Opera 5 can't deal with a simple background: transparent) -->

a:link {
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		color: <xsl:value-of select="$appearance/app:colours/app:dark"/>;
		background-color: transparent;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		color: #000000;
	</xsl:if>
}
a:visited {
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		color: <xsl:value-of select="$appearance/app:colours/app:medium"/>;
		background-color: transparent;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		color: #000000;
	</xsl:if>
}
<xsl:if test="contains ( 'screen, handheld', $medium )">
	a:link:hover, a:visited:hover {
		color: <xsl:value-of select="$appearance/app:colours/app:page"/>;
		background: <xsl:value-of select="$appearance/app:colours/app:dark"/>;
	}
</xsl:if>
<!-- named elements -->
#AccessLinks {
	<xsl:if test="contains ( 'screen', $medium )">
		padding: 0 1em 0 3em;
		<!-- would use margin, but IE/Win ballses up -->
		float: left;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
#ReferenceLinks {
	<xsl:if test="contains ( 'screen', $medium )">
		padding: 0 6em 0 1em;
		<!-- would use margin, but IE/Win ballses up -->
		text-align: right;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
#Breadcrumb {
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
#TextSize {
	<xsl:if test="contains ( 'print, handheld', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: inline;
	</xsl:if>
}
#CuneiformSwitcher {
	<xsl:if test="contains ( 'print, handheld', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: inline;
	</xsl:if>
}

#Header {
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		<xsl:if test="contains ( 'screen', $medium )">
			padding: 0.5em 3em;
		</xsl:if>
		<xsl:if test="contains ( 'handheld', $medium )">
			padding: 0.5em 1em;
		</xsl:if>
		background: <xsl:value-of select="$appearance/app:colours/app:dark"/>;
	</xsl:if>
	<xsl:if test="contains ( 'handheld', $medium )">
		margin: 1em 0;
	</xsl:if>
	<xsl:if test="contains ( 'screen', $medium )">
		margin: 0.3em 0 0;
		height: 2.75em;
		border-bottom: 0.5em solid <xsl:value-of select="$appearance/app:colours/app:light"/>;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		border-bottom: 1px solid #000000;
		margin: 0 1em 0 1em;
	</xsl:if>
}
#HeadTitle {
	<xsl:if test="contains ( 'screen', $medium )">
		font-weight: bold; font-size: 1.6em;
	</xsl:if>
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		color: <xsl:value-of select="$appearance/app:colours/app:page"/>;
	</xsl:if>
	<xsl:if test="contains ( 'print, handheld', $medium )">
		font-weight: bold;
	</xsl:if>
}
#HeadTitle a:link, #HeadTitle a:visited {
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		color: <xsl:value-of select="$appearance/app:colours/app:page"/>;
		background: <xsl:value-of select="$appearance/app:colours/app:dark"/>;
	</xsl:if>
	text-decoration: none;
}
<!-- this was removed from the 2009 stylesheets.xslt; perhaps it is not used anywhere? -->
#HeadSubtitle {
	<xsl:if test="contains ( 'screen', $medium )">
		font-style: italic; font-weight: normal; font-size: 1.2em;<!--rm 1.6em originally-->
	</xsl:if>
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		color: <xsl:value-of select="$appearance/app:colours/app:page"/>;
	</xsl:if>
	<xsl:if test="contains ( 'print, handheld', $medium )">
		font-weight: normal;
	</xsl:if>
}
#Search {
	<xsl:if test="contains ( 'screen', $medium )">
		float: right;
		padding: 0.5em 6em 0.5em 1em;
		margin: 0.3em 0 0 -3px;
		<!-- the above -3px fixes annoying IE Win bug -->
		background: <xsl:value-of select="$appearance/app:colours/app:dark"
                    /> url(<xsl:value-of select="concat($parameters/param:root,'/',
                                                        $appearance/app:images/app:dark-corner)"
                                       />) no-repeat top right;
		height: 2.75em;
		border-bottom: 0.5em solid <xsl:value-of select="$appearance/app:colours/app:light"/>;
	</xsl:if>
	<xsl:if test="contains ( 'print, handheld', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
<xsl:if test="contains ( 'screen', $medium )">
	#Search label {
		font-weight: bold;
		color: <xsl:value-of select="$appearance/app:colours/app:page"/>;
	}
</xsl:if>

#Menu {
	<xsl:if test="contains ( 'screen', $medium )">
		float: left;
		width: 16.5em;
		margin: 0.8em 0 1em 0;
	</xsl:if>
	<xsl:if test="contains ( 'handheld', $medium )">
		margin: 1em 0.5em;
	</xsl:if>
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		padding: 0;
		background: <xsl:value-of select="$appearance/app:colours/app:page"/>;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
<xsl:if test="contains ( 'screen, handheld', $medium )">
	#MenuCaption {
		<xsl:if test="contains ( 'screen', $medium )">
			margin-left: 3em;
			margin-bottom: 0.5em;
			padding-left: 0;
			font-size: 0.9em;
			font-weight: bold;
		</xsl:if>
		<xsl:if test="contains ( 'handheld', $medium )">
			padding: 0 0 0 1em;
		</xsl:if>
		border-bottom: 2px solid <xsl:value-of select="$appearance/app:colours/app:light"/>;
	}
	#Menu a {
		text-decoration: none;
	}
	#Menu ul {
		padding: 0;
		margin: 0;
	}
	#Menu ul ul {
		<xsl:if test="contains ( 'screen', $medium )">
			padding: 0.25em 0 0 4em;
			margin-bottom: 0.25em;
		</xsl:if>
		<xsl:if test="contains ( 'handheld', $medium )">
			padding: 0 0 0 1em;
		</xsl:if>
		border: 1px solid <xsl:value-of select="$appearance/app:colours/app:light"/>;
		border-top: 0;
	}
	#Menu ul ul ul {
		border-right: 0;
		padding: 0.25em 0 0 1em;
	}
	#Menu li {
		padding: 0;
		margin: 0;
		list-style-image: url(/img/blank.gif);
		/* IE Win does weird stuff with list-style-type: none */
	}
	<xsl:if test="contains ( 'screen', $medium )">
		<!-- #Menu li.closed, #Menu li.open  {
			font-weight: bold;
		} -->
		#Menu li.only  {
			font-weight: normal;
		}
	</xsl:if>
	#Menu ul a, ul #SelfInMenu {
		display: block;
		<xsl:if test="contains ( 'screen', $medium )">
			padding: 0.35em 1em 0.35em 3em;
		</xsl:if>
		<xsl:if test="contains ( 'handheld', $medium )">
			padding: 0.35em 1em 0.35em 1em;
		</xsl:if>
		line-height: normal;
	}
	#Menu ul ul a, ul ul #SelfInMenu {
		padding: 0.35em 1em 0.35em 1em;
	}
	#Menu a.open:hover, #Menu a.closed:hover  {
		color: <xsl:value-of select="$appearance/app:colours/app:medium"/>;
		background: <xsl:value-of select="$appearance/app:colours/app:highlight"
                    /> url(<xsl:value-of select="concat($parameters/param:root,'/',
                                                        $appearance/app:images/app:highlight-corner)"
                                        />) no-repeat top right;
	}  <!--colours for #Menu a.open:hover, #Menu a.closed:hover were esp:dark (background) and esp:page (color) but it seemed too harsh-->
	#Menu a.open {
		background: <xsl:value-of select="$appearance/app:colours/app:light"
                    /> url(<xsl:value-of select="concat($parameters/param:root,'/',
                                                        $appearance/app:images/app:light-corner)"
                                        />) no-repeat top right;
	}
	#SelfInMenu {
		font-weight: bold;
	}
	#SelfInMenu.open {
		background: <xsl:value-of select="$appearance/app:colours/app:light"
                    /> url(<xsl:value-of select="concat($parameters/param:root,'/',
                                                        $appearance/app:images/app:light-corner)"
                                        />) no-repeat top right;
	}
</xsl:if>

#StyleSheetWarning {
	display: none;
}
#Content {
	<xsl:if test="contains ( 'screen', $medium )">
		margin: 1em 6em 0 18em;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		margin: 2em;
	</xsl:if>
}
<xsl:if test="contains ( 'screen, print, handheld', $medium )">
	#SiteMap a {
		text-decoration: none;
	}
	#SiteMap ul {
		margin: 0;
		padding-left: 2em;
		list-style: none;
	}
	#SiteMap > ul {
		padding-left: 0;
	}
	#SiteMap h2 {
		margin: 0.3em 0;
	}
	#SiteMap h3 {
		margin: 0.2em 0;
	}
	#SiteMap div {
		margin: 0.1em 0;
	}
	#AccessKeys {
		border: 0;
		margin: 0 1em;
		padding: 0;
	}
	#AccessKeys th, #AccessKeys td {
		padding: 0 1em;
		text-align: left;
	}
</xsl:if>
#AccessKeys th {
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		border-bottom: solid 2px <xsl:value-of select="$appearance/app:colours/app:text"/>;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		border-bottom: solid 2px #000000;
	</xsl:if>
}
<xsl:if test="contains ( 'screen, handheld, print', $medium )">
	#AccessKeys tr.odd {
		background: <xsl:value-of select="$appearance/app:colours/app:alternate"/>;
	}
	#Index h2, #Glossary h2, #Techterms h2 {
		float: left;
		margin: 0;
		color: <xsl:value-of select="$appearance/app:colours/app:outline"/>;
	}
	#Vocablist h2 {
		margin: 0;
		line-height: 1.3em;
		color: <xsl:value-of select="$appearance/app:colours/app:text"/>;
	}
	#Vocablist h2.empty {
		margin: 0 0 2.5em;
		line-height: 1.3em;
		color: <xsl:value-of select="$appearance/app:colours/app:outline"/>;
	}
	#Index h3, #Glossary dl, #Techterms dl  {
		margin: 0 0 0 3em;
	}
	#Index ul, #Vocablist ul {
		list-style-type: none;
		margin: 0 0 1em 4em;
		padding: 0;
	}
	#Glossary dt {
		font-weight: bold;
	}
	#Techterms dt {
		font-weight: bold;
	}
	#Authors {
		clear: both;
		<xsl:if test="contains ( 'screen, handheld', $medium )">
			border-top: dotted 1px <xsl:value-of select="$appearance/app:colours/app:dark"/>;
		</xsl:if>
		<xsl:if test="contains ( 'print', $medium )">
			border-top: dotted 1px <xsl:value-of select="$appearance/app:colours/app:print"/>;
		</xsl:if>
		margin: 2.5em 0 0;
		padding: 1em 0 0;
		font-style: italic;
		font-size: 0.9em;
	}
	#CiteAs {
		font-size: 0.9em;
	}
</xsl:if>
#Alphabet {
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		margin: 0 0 1em;
		color: <xsl:value-of select="$appearance/app:colours/app:outline"/>;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
<xsl:if test="contains ( 'screen, handheld', $medium )">
#Alphabet a {
	text-decoration: none;
}
</xsl:if>
#PageLinks {
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: inline;
	</xsl:if>
}
#BackToTop {
	<xsl:if test="contains ( 'screen', $medium )">
		text-align: right;
		margin: 0 6em;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
<xsl:if test="contains ( 'screen, print', $medium )">
	#EndContentSpace {
		clear: both;
		height: 1em;
	}
</xsl:if>

#FooterWhole {
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		border-bottom: solid 1px <xsl:value-of select="$appearance/app:colours/app:dark"/>;
		margin: 0 0 0.2em;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		border-bottom: solid 1px #000000;
	</xsl:if>
	<xsl:if test="contains ( 'screen, print', $medium )">
		clear: both;
	</xsl:if>
	<xsl:if test="contains ( 'screen, print, handheld', $medium )">
		height: 1em;
	</xsl:if>
}

#FooterLeft {
	<xsl:if test="contains ( 'screen', $medium )">
		padding: 0 1em 0 3em;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		padding: 0 1em;
	</xsl:if>
}
#FooterRight {
	<xsl:if test="contains ( 'screen', $medium )">
		float: right;
		padding: 0 6em 0 1em;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
<!--
<xsl:if test="contains ( 'screen', $medium )">
	#tab_cams {
		position: absolute;
		top: 7.5em;
		margin-top: 8px;
		right: 0;
	}
	#tab_knp {
		position: absolute;
		top: 7.5em;
		margin-top: 126px;
		right: 0;
	}
	#tab_saa {
		position: absolute;
		top: 7.5em;
		margin-top: 244px;
		right: 0;
	}
	#tab_cams img, #tab_knp img, #tab_saa img {
		display: block;
	}
</xsl:if>
#tab_cams, #tab_knp, #tab_saa {
	<xsl:if test="contains ( 'print, handheld', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}
-->
<xsl:if test="contains('screen',$medium)">
  <xsl:call-template name="tab-css"/>
  <xsl:call-template name="tab-list-ids">
    <xsl:with-param name="arg" select="'img'"/>
  </xsl:call-template> {
		display: block;
	}
</xsl:if>
<xsl:call-template name="tab-list-ids"/> {
	<xsl:if test="contains ( 'print, handheld', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'browsersthatsuck', $medium )">
		display: block;
	</xsl:if>
}

#URL {
	<xsl:if test="contains ( 'screen, handheld', $medium )">
		display: none;
	</xsl:if>
	<xsl:if test="contains ( 'print', $medium )">
		margin: 0 1em;
	</xsl:if>
}

</xsl:template>

<xsl:template name="tab-css">
  <xsl:for-each select="$appearance/app:tabs/*">
    <xsl:variable name="margin-top">
      <xsl:call-template name="tab-margin-top"/>
     </xsl:variable>
	#tab_<xsl:value-of select="position()"/> {
		position: absolute;
		top: 7.5em;
		margin-top: <xsl:value-of select="$margin-top"/>px;
		right: 0;
	}
  </xsl:for-each>
</xsl:template>

<xsl:template name="tab-margin-top">
  <xsl:value-of select="8 + sum(preceding-sibling::*/app:img/@height) + count(preceding-sibling::*)"/>
</xsl:template>

<xsl:template name="tab-list-ids">
  <xsl:param name="arg"/>
  <xsl:for-each select="$appearance/app:tabs/*">
    <xsl:value-of select="concat('#tab_',position())"/>
    <xsl:if test="string-length($arg)">
      <xsl:value-of select="concat(' ', $arg)"/>
    </xsl:if>
    <xsl:if test="not(position()=last())">
      <xsl:text>, </xsl:text>
    </xsl:if>
  </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
