declare namespace exist = "http://exist.sourceforge.net/NS/exist";
declare namespace request = "http://exist-db.org/xquery/request";

declare namespace gdl  = "http://oracc.org/ns/gdl/1.0";
declare namespace norm = "http://oracc.org/ns/norm/1.0";
declare namespace xcl  = "http://oracc.org/ns/xcl/1.0";
declare namespace xff  = "http://oracc.org/ns/xff/1.0";
declare namespace xtf  = "http://oracc.org/ns/xtf/1.0";

declare variable $project as xs:string := request:get-parameter('project','');
declare variable $P as document-node()* := collection(concat('/db/',$project,'/oml'));
declare option exist:serialize 
        "method=xml media-type=text/xml omit-xml-declaration=no indent=yes";

document {

processing-instruction
  xml-stylesheet
  { 'href="http://oracc.museum.upenn.edu/css/stats-xql.css" type="text/css"' },

<stats>
<title>Statistics for project { $project }</title>
<count>
  <what>Number of graphemes</what>
  <number>{
      count($P/(xtf:transliteration|xtf:composite)//gdl:v)
  }</number>
</count>
<count>
  <what>Number of words</what>
  <number>{
      count($P/(xtf:transliteration|xtf:composite)//*
		[self::gdl:w or self::norm:w])
  }</number>
</count>
<count>
  <what>Number of words written with one grapheme</what>
  <number>{
      count($P/(xtf:transliteration|xtf:composite)//*
	    [self::gdl:w][count(*)=1])
  }</number>
</count>
<count>
  <what>Number of lemmata</what>
  <number>{
      count($P/(xtf:transliteration|xtf:composite)//xcl:l)
  }</number>
</count>
<count>
  <what>Number of lemmata with Citation Form</what>
  <number>{
      count($P/(xtf:transliteration|xtf:composite)//xcl:l[xff:f/@cf])
  }</number>
</count>
</stats>

}
