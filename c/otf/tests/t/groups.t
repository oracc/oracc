001. %a A
     A
     <g:gg g:type="logo"><g:s g:logolang="sux" g:role="logo">A</g:s></g:gg>

002. {D}a
     {D}a
     <g:d g:pos="pre" g:role="semantic"><g:s>D</g:s></g:d><g:v>a</g:v>

003. %a {D}a
     {D}a
     <g:d g:pos="pre" g:role="semantic">
       <g:gg g:type="logo">
         <g:s g:logolang="sux" g:role="logo">D</g:s>
       </g:gg>
     </g:d>
     <g:v>a</g:v>

004. %a {D}A
     {D}A
     <g:gg g:type="logo">
       <g:d g:pos="pre" g:role="semantic">
         <g:gg g:type="logo">
           <g:s g:logolang="sux" g:role="logo">D</g:s>
         </g:gg>
       </g:d>
       <g:s g:logolang="sux" g:role="logo">A</g:s>
     </g:gg>

005. %a {d}+EN.KAD
     {d}+EN.KAD
     <g:gg g:type="logo">
       <g:gg g:type="ligature">
         <g:d g:role="semantic" g:pos="pre" g:delim="+">
           <g:v>d</g:v>
         </g:d>
	 <g:s g:logolang="sux" g:role="logo" g:delim=".">EN</g:s>
       </g:gg>
       <g:s g:logolang="sux" g:role="logo">KAD</g:s>
     </g:gg>

006. %a {d}+EN-KAD
     {d}+EN-KAD
     <g:gg g:type="logo">
       <g:gg g:type="ligature">
         <g:d g:role="semantic" g:pos="pre" g:delim="+">
           <g:v>d</g:v>
         </g:d>
	 <g:s g:logolang="sux" g:role="logo" g:delim="-">EN</g:s>
       </g:gg>
       <g:s g:logolang="sux" g:role="logo">KAD</g:s>
     </g:gg>
     
007. %a {m}{d}+EN.KAD
     {m}{d}+EN.KAD
     <g:d g:pos="pre" g:role="semantic">
       <g:v>m</g:v>
     </g:d>
     <g:gg g:type="logo">
       <g:gg g:type="ligature">
         <g:d g:role="semantic" g:pos="pre" g:delim="+">
           <g:v>d</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo" g:delim=".">EN</g:s>
       </g:gg>
       <g:s g:logolang="sux" g:role="logo">KAD</g:s>
     </g:gg>
	      
008. %a {m}{d}+EN-KAD
     {m}{d}+EN-KAD
     <g:d g:pos="pre" g:role="semantic">
       <g:v>m</g:v>
     </g:d>
     <g:gg g:type="logo">
       <g:gg g:type="ligature">
         <g:d g:role="semantic" g:pos="pre" g:delim="+">
           <g:v>d</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo" g:delim="-">EN</g:s>
       </g:gg>
       <g:s g:logolang="sux" g:role="logo">KAD</g:s>
     </g:gg>

009. %a {gis}TUKUL.{gis}TUKUL-su
     {gis}TUKUL.{gis}TUKUL-su
     <g:gg g:type="logo">
       <g:d g:pos="pre" g:role="semantic">
         <g:v>gis</g:v>
       </g:d>
       <g:s g:delim="." g:logolang="sux" g:role="logo">TUKUL</g:s>
       <g:d g:pos="pre" g:role="semantic">
         <g:v>gis</g:v>
       </g:d>
       <g:s g:delim="-" g:logolang="sux" g:role="logo">TUKUL</g:s>
     </g:gg>
     <g:v>su</g:v>

010. %a x+x{KI}
   x+x{KI}
   <g:gg g:type="ligature">
     <g:v g:delim="+">x</g:v>
     <g:v>x</g:v>
     <g:d g:role="semantic" g:pos="post">
       <g:s g:logolang="sux" g:role="logo">KI</g:s>
     </g:d>
   </g:gg>

011. %a MIN<(su-ri-in-ni)>
   su-ri-in-ni
   <surro>
     <g:nonw xml:lang="akk" type="surro">
       <g:s g:role="logo" g:logolang="sux">MIN</g:s>
     </g:nonw>
     <g:w xml:lang="akk" form="su-ri-in-ni">
        <g:v g:delim="-">su</g:v>
        <g:v g:delim="-">ri</g:v>
        <g:v g:delim="-">in</g:v>
        <g:v>ni</g:v>
     </g:w>
   </surro>
