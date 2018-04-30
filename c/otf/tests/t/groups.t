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
     <g:w form="{d}+EN.KAD" xml:lang="akk">
       <g:gg g:type="logo">
         <g:gg g:delim="." g:type="ligature">
           <g:d g:delim="+" g:pos="pre" g:role="semantic">
             <g:v>d</g:v>
           </g:d>
           <g:s g:logolang="sux" g:role="logo">EN</g:s>
         </g:gg>
         <g:s g:logolang="sux" g:role="logo">KAD</g:s>
       </g:gg>
     </g:w>

006. %a {d}+EN-KAD
     {d}+EN-KAD
     <g:w form="{d}+EN-KAD" xml:lang="akk">
       <g:gg g:type="logo">
         <g:gg g:type="ligature">
           <g:d g:delim="+" g:pos="pre" g:role="semantic">
             <g:v>d</g:v>
           </g:d>
           <g:s g:delim="-" g:logolang="sux" g:role="logo">EN</g:s>
         </g:gg>
         <g:s g:logolang="sux" g:role="logo">KAD</g:s>
       </g:gg>
     </g:w>
     
007. %a {m}{d}+EN.KAD
     {m}{d}+EN.KAD
     <g:w form="{m}{d}+EN.KAD" xml:lang="akk">
       <g:d g:pos="pre" g:role="semantic">
         <g:v>m</g:v>
       </g:d>
       <g:gg g:type="logo">
         <g:gg g:delim="." g:type="ligature">
           <g:d g:delim="+" g:pos="pre" g:role="semantic">
             <g:v>d</g:v>
           </g:d>
           <g:s g:logolang="sux" g:role="logo">EN</g:s>
         </g:gg>
         <g:s g:logolang="sux" g:role="logo">KAD</g:s>
       </g:gg>
     </g:w>
	      
008. %a {m}{d}+EN-KAD
     {m}{d}+EN-KAD
     <g:w form="{m}{d}+EN-KAD" xml:lang="akk">
       <g:d g:pos="pre" g:role="semantic">
         <g:v>m</g:v>
       </g:d>
       <g:gg g:type="logo">
         <g:gg g:type="ligature">
           <g:d g:delim="+" g:pos="pre" g:role="semantic">
             <g:v>d</g:v>
           </g:d>
           <g:s g:delim="-" g:logolang="sux" g:role="logo">EN</g:s>
         </g:gg>
         <g:s g:logolang="sux" g:role="logo">KAD</g:s>
       </g:gg>
     </g:w>

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
     <g:d g:pos="post" g:role="semantic">
       <g:gg g:type="logo">
         <g:s g:logolang="sux" g:role="logo">KI</g:s>
       </g:gg>
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
         <g:d g:delim="-" g:pos="post" g:role="phonetic">
           <g:v g:delim="-">qu</g:v>
           <g:v>u</g:v>
         </g:d>
       </g:gg>
       <g:v>u</g:v>
     </g:w>

014. %a {lu}DU{+u}-ME
     {lu}DU{+u}-ME
     <g:w form="{lu}DU{+u}-ME" xml:lang="akk">
       <g:gg g:type="logo">
         <g:d g:pos="pre" g:role="semantic">
           <g:v>lu</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo">DU</g:s>
         <g:d g:delim="-" g:pos="post" g:role="phonetic">
           <g:v>u</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo">ME</g:s>
       </g:gg>
     </g:w>

015. %a {f}GEME{+ti}-DUMU
     {f}GEME{+ti}-DUMU
     <g:w form="{f}GEME{+ti}-DUMU" xml:lang="akk">
       <g:gg g:type="logo">
         <g:d g:pos="pre" g:role="semantic">
           <g:v>f</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo">GEME</g:s>
         <g:d g:delim="-" g:pos="post" g:role="phonetic">
           <g:v>ti</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo">DUMU</g:s>
       </g:gg>
     </g:w>

016. %a [...]-na
     x-na
     <g:w form="x-na" xml:lang="akk">
       <g:x g:break="missing" g:breakEnd="X000001.1.1.0" g:breakStart="1" g:c="]" g:delim="-" g:o="[" g:type="ellipsis">...</g:x>
       <g:v>na</g:v>
     </g:w>

017. %a {lu}GIR.LA{+tu!(SAR)}-ME
     {lu}GIR.LA{+tu}-ME
     <g:w form="{lu}GIR.LA{+tu}-ME" xml:lang="akk">
       <g:gg g:type="logo">
         <g:d g:pos="pre" g:role="semantic">
           <g:v>lu</g:v>
         </g:d>
         <g:s g:delim="." g:logolang="sux" g:role="logo">GIR</g:s>
         <g:s g:logolang="sux" g:role="logo">LA</g:s>
         <g:d g:delim="-" g:pos="post" g:role="phonetic">
           <g:gg g:type="correction">
             <g:v g:remarked="1">tu</g:v>
             <g:s>SAR</g:s>
           </g:gg>
         </g:d>
         <g:s g:logolang="sux" g:role="logo">ME</g:s>
       </g:gg>
     </g:w>


018. %a A--U
     A-U
     <g:gg g:type="logo">
       <g:s g:delim="—" g:em="1" g:logolang="sux" g:role="logo">A</g:s>
       <g:s g:logolang="sux" g:role="logo">U</g:s>
     </g:gg>

019. %a {KUR}ELAM.MA{KI}-ma
     {KUR}ELAM.MA{KI}-ma
     <g:w form="{KUR}ELAM.MA{KI}-ma" xml:lang="akk">
       <g:gg g:type="logo">
         <g:d g:pos="pre" g:role="semantic">
           <g:gg g:type="logo">
             <g:s g:logolang="sux" g:role="logo">KUR</g:s>
           </g:gg>
         </g:d>
         <g:s g:delim="." g:logolang="sux" g:role="logo">ELAM</g:s>
         <g:s g:logolang="sux" g:role="logo">MA</g:s>
         <g:d g:delim="-" g:pos="post" g:role="semantic">
           <g:s g:logolang="sux" g:role="logo">KI</g:s>
         </g:d>
       </g:gg>
       <g:v>ma</g:v>
     </g:w>

020. %a aš-šur{KI}-ma
     aš-šur{KI}-ma
     <g:w form="aš-šur{KI}-ma" xml:lang="akk">
       <g:v g:delim="-">aš</g:v>
       <g:v>šur</g:v>
       <g:d g:delim="-" g:pos="post" g:role="semantic">
         <g:gg g:type="logo">
           <g:s g:logolang="sux" g:role="logo">KI</g:s>
         </g:gg>
       </g:d>
       <g:v>ma</g:v>
     </g:w>

021. %a u{{u-a}}
     u
     <g:w form="u" xml:lang="akk">
       <g:v>u</g:v>
       <g:gloss g:pos="post" g:type="lang">
         <g:w form="u-a" xml:lang="akk">
           <g:v g:delim="-">u</g:v>
           <g:v>a</g:v>
         </g:w>
       </g:gloss>
     </g:w>

022. %a U{{u-a}}
     U
     <g:w form="U" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:logolang="sux" g:role="logo">U</g:s>
       </g:gg>
       <g:gloss g:pos="post" g:type="lang">
         <g:w form="u-a" xml:lang="akk">
           <g:v g:delim="-">u</g:v>
           <g:v>a</g:v>
         </g:w>
       </g:gloss>
     </g:w>

023. %a E;.A
     E.A
     <g:w form="E.A" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:logolang="sux" g:role="logo">E</g:s>
         <g:x g:delim="." g:type="newline"></g:x>
         <g:s g:logolang="sux" g:role="logo">A</g:s>
       </g:gg>
     </g:w>

024. %a UD.x.KAM
     UD.x.KAM
     <g:w form="UD.x.KAM" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:delim="." g:logolang="sux" g:role="logo">UD</g:s>
         <g:v g:delim=".">x</g:v>
         <g:s g:logolang="sux" g:role="logo">KAM</g:s>
       </g:gg>
     </g:w>
