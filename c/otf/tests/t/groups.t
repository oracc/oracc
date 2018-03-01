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
         <g:d g:delim="+" g:pos="pre" g:role="semantic">
           <g:v>d</g:v>
         </g:d>
	 <g:s g:delim="." g:logolang="sux" g:role="logo">EN</g:s>
       </g:gg>
       <g:s g:logolang="sux" g:role="logo">KAD</g:s>
     </g:gg>

006. %a {d}+EN-KAD
     {d}+EN-KAD
     <g:gg g:type="logo">
       <g:gg g:type="ligature">
         <g:d g:delim="+" g:pos="pre" g:role="semantic">
           <g:v>d</g:v>
         </g:d>
	 <g:s g:delim="-" g:logolang="sux" g:role="logo">EN</g:s>
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
         <g:d g:delim="+" g:role="semantic" g:pos="pre">
           <g:v>d</g:v>
         </g:d>
         <g:s g:delim="." g:logolang="sux" g:role="logo">EN</g:s>
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
         <g:d g:delim="+"" g:pos="pre" g:role="semantic">
           <g:v>d</g:v>
         </g:d>
         <g:s g:delim="-" g:logolang="sux" g:role="logo">EN</g:s>
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

011. %a MIN<(su-ri)>
   su-ri
   <x:surro>
     <g:nonw type="surro" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:logolang="sux" g:role="logo">MIN</g:s>
       </g:gg>
     </g:nonw>
     <g:w form="su-ri" xml:lang="akk">
        <g:v g:delim="-">su</g:v>
        <g:v>ri</g:v>
     </g:w>
   </x:surro>

012. %a {d}MIN<(nin-urta)>
     {d}nin-urta
     <g:w form="{d}nin-urta" xml:lang="akk">
       <g:d g:pos="pre" g:role="semantic">
         <g:v>d</g:v>
       </g:d>
       <g:surro>
         <g:gg g:type="logo">
           <g:s g:logolang="sux" g:role="logo">MIN</g:s>
         </g:gg>
         <g:gg g:surroStart="1" g:type="group">
           <g:v g:delim="-">nin</g:v>
           <g:v g:surroEnd="X000001.1.1.2">urta</g:v>
         </g:gg>
       </g:surro>
     </g:w>

013. %a GU{+qu-u}-u
     GU{+qu-u}-u
     <g:w form="GU{+qu-u}-u" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:logolang="sux" g:role="logo">GU</g:s>
         <g:d g:pos="post" g:role="phonetic">
           <g:v g:delim="-">qu</g:v>
           <g:v g:delim="-">u</g:v>
         </g:d>
       </g:gg>
       <g:v>u</g:v>
     </g:w>
