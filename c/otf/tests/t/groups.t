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
         <g:gg g:type="ligature">
           <g:d g:delim="+" g:pos="pre" g:role="semantic">
             <g:v>d</g:v>
           </g:d>
           <g:s g:delim="." g:logolang="sux" g:role="logo">EN</g:s>
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
         <g:gg g:type="ligature">
           <g:d g:delim="+" g:pos="pre" g:role="semantic">
             <g:v>d</g:v>
           </g:d>
           <g:s g:delim="." g:logolang="sux" g:role="logo">EN</g:s>
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
   <g:w form="su-ri" xml:lang="akk">
     <g:surro>
       <g:s>MIN</g:s>
       <g:gg g:type="group">
         <g:v g:delim="-">su</g:v>
         <g:v g:surroEnd="X000001.1.1.0">ri</g:v>
       </g:gg>
     </g:surro>
   </g:w>

012. %a {d}MIN<(nin-urta)>
     {d}nin-urta
     <g:w form="{d}nin-urta" xml:lang="akk">
       <g:d g:pos="pre" g:role="semantic">
         <g:v>d</g:v>
       </g:d>
       <g:surro>
         <g:s>MIN</g:s>
         <g:gg g:type="group">
           <g:v g:delim="-">nin</g:v>
           <g:v g:surroEnd="X000001.1.1.1">urta</g:v>
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

025. %a {mul}AL.($ o $)LUL
     {mul}AL.LUL
     <g:w form="{mul}AL.LUL" xml:lang="akk">
       <g:gg g:type="logo">
         <g:d g:pos="pre" g:role="semantic">
           <g:v>mul</g:v>
         </g:d>
         <g:s g:delim="." g:logolang="sux" g:role="logo">AL</g:s>
         <g:x g:type="dollar"> o </g:x>
         <g:s g:logolang="sux" g:role="logo">LUL</g:s>
       </g:gg>
     </g:w>

026. %a {+MIN<(a)>}{{ku}}20
     {+a}20
     <g:w form="{+a}20" xml:lang="akk">
       <g:d g:pos="pre" g:role="phonetic">
         <g:surro>
           <g:s>MIN</g:s>
           <g:gg g:type="group">
             <g:v g:surroEnd="X000001.1.1.0">a</g:v>
           </g:gg>
         </g:surro>
       </g:d>
       <g:gloss g:pos="pre" g:type="lang">
         <g:w form="ku" xml:lang="akk">
           <g:v>ku</g:v>
         </g:w>
       </g:gloss>
       <g:n form="20" sexified="2(u)">
         <g:r>20</g:r>
       </g:n>
     </g:w>

027. %a {m}aš+šur--EN
     {m}aš+šur-EN
     <g:w form="{m}aš+šur-EN" xml:lang="akk">
       <g:d g:pos="pre" g:role="semantic">
         <g:v>m</g:v>
       </g:d>
       <g:gg g:type="logo">
         <g:gg g:type="ligature">
           <g:v g:delim="+">aš</g:v>
           <g:v g:delim="—" g:em="1">šur</g:v>
         </g:gg>
         <g:s g:logolang="sux" g:role="logo">EN</g:s>
       </g:gg>
     </g:w>

028. %a {m}aš+šur--EN--GUB
     {m}aš+šur-EN-GUB
     <g:w form="{m}aš+šur-EN-GUB" xml:lang="akk">
       <g:d g:pos="pre" g:role="semantic">
         <g:v>m</g:v>
       </g:d>
       <g:gg g:type="logo">
         <g:gg g:type="ligature">
           <g:v g:delim="+">aš</g:v>
           <g:v g:delim="—" g:em="1">šur</g:v>
         </g:gg>
         <g:s g:delim="—" g:em="1" g:logolang="sux" g:role="logo">EN</g:s>
         <g:s g:logolang="sux" g:role="logo">GUB</g:s>
       </g:gg>
     </g:w>

029. %a I₃+GIŠ
     I₃+GIŠ
     <g:w form="I₃+GIŠ" xml:lang="akk">
       <g:gg g:type="logo">
         <g:gg g:type="ligature">
           <g:s g:delim="+" g:logolang="sux" g:role="logo">I₃</g:s>
	   <g:s g:logolang="sux" g:role="logo">GIŠ</g:s>
         </g:gg>
       </g:gg>
     </g:w>

030. %a UD-TI.LA{+su}-KA
     UD-TI.LA{+su}-KA
     <g:w form="UD-TI.LA{+su}-KA" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:delim="-" g:logolang="sux" g:role="logo">UD</g:s>
         <g:s g:delim="." g:logolang="sux" g:role="logo">TI</g:s>
         <g:s g:logolang="sux" g:role="logo">LA</g:s>
         <g:d g:delim="-" g:pos="post" g:role="phonetic">
           <g:v>su</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo">KA</g:s>
       </g:gg>
     </g:w>

031. %a {d}UR{ki}.GI
     {d}UR{ki}.GI
     <g:w form="{d}UR{ki}.GI" xml:lang="akk">
       <g:gg g:type="logo">
         <g:d g:pos="pre" g:role="semantic">
           <g:v>d</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo">UR</g:s>
         <g:d g:pos="post" g:role="semantic">
           <g:v g:delim=".">ki</g:v>
         </g:d>
         <g:s g:logolang="sux" g:role="logo">GI</g:s>
       </g:gg>
     </g:w>

032. %a |A.E|-I
     |A.E|-I
     <g:w form="|A.E|-I" xml:lang="akk">
       <g:gg g:type="logo">
         <g:c form="|A.E|" g:delim="-" g:logolang="sux" g:role="logo">
           <g:s>A</g:s>
           <g:o g:type="beside"></g:o>
           <g:s>E</g:s>
         </g:c>
         <g:s g:logolang="sux" g:role="logo">I</g:s>
       </g:gg>
     </g:w>

    <g:q form="en(|PA.TE|)">
      <g:v>en</g:v>
      <g:c form="|PA.TE|" g:delim="-">
        <g:s>PA</g:s>
        <g:o g:type="beside"/>
        <g:s>TE</g:s>
      </g:c>
    </g:q>
    <g:v>gal</g:v>

033. %a en(|PA.TE|)-gal
     en(|PA.TE|)-gal
     <g:w form="en(|PA.TE|)-gal" xml:lang="akk">
       <g:q form="en(|PA.TE|)" g:delim="-">
         <g:v>en</g:v>
         <g:c form="|PA.TE|">
           <g:s>PA</g:s>
           <g:o g:type="beside"></g:o>
           <g:s>TE</g:s>
         </g:c>
       </g:q>
       <g:v>gal</g:v>
     </g:w>

034. %a SIG₂{HI.A}
     SIG₂{HI.A}
     <g:w form="SIG₂{HI.A}" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:logolang="sux" g:role="logo">SIG₂</g:s>
         <g:d g:pos="post" g:role="semantic">
           <g:s g:delim="." g:logolang="sux" g:role="logo">HI</g:s>
           <g:s g:logolang="sux" g:role="logo">A</g:s>
         </g:d>
       </g:gg>
     </g:w>

035. a:zu
     a-zu
     <g:w form="a-zu" xml:lang="sux">
       <g:gg g:type="reordering">
         <g:v g:delim=":">a</g:v>
         <g:v>zu</g:v>
       </g:gg>
     </g:w>

036. %a E.MIN<(NIN.URTA)>
     E.NIN.URTA
     <g:w form="E.NIN.URTA" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:delim="." g:logolang="sux" g:role="logo">E</g:s>
         <g:surro>
           <g:s>MIN</g:s>
           <g:gg g:type="group">
             <g:s g:delim="." g:logolang="sux" g:role="logo">NIN</g:s>
             <g:s g:logolang="sux" g:role="logo" g:surroEnd="X000001.1.1.1">URTA</g:s>
           </g:gg>
         </g:surro>
       </g:gg>
     </g:w>

037. %a MIN#<(IN.NIN)>
     IN.NIN
     <g:w form="IN.NIN" xml:lang="akk">
       <g:surro>
         <g:s g:break="damaged" g:hc="1" g:ho="1">MIN</g:s>
         <g:gg g:type="group">
           <g:gg g:type="logo">
             <g:s g:delim="." g:logolang="sux" g:role="logo">IN</g:s>
             <g:s g:logolang="sux" g:role="logo" g:surroEnd="X000001.1.1.0">NIN</g:s>
           </g:gg>
         </g:gg>
       </g:surro>
     </g:w>

038. %a {d}A.NUN.NA
     {d}A.NUN.NA
     <g:w form="{d}A.NUN.NA" xml:lang="akk">
       <g:gg g:type="logo">
         <g:d g:pos="pre" g:role="semantic">
           <g:v>d</g:v>
         </g:d>
         <g:s g:delim="." g:logolang="sux" g:role="logo">A</g:s>
         <g:s g:delim="." g:logolang="sux" g:role="logo">NUN</g:s>
         <g:s g:logolang="sux" g:role="logo">NA</g:s>
       </g:gg>
     </g:w>

039. %a DINGIR-ME
     DINGIR-ME
     <g:w form="DINGIR-ME" xml:lang="akk">
       <g:gg g:type="logo">
         <g:s g:delim="-" g:logolang="sux" g:role="logo">DINGIR</g:s>
         <g:s g:logolang="sux" g:role="logo">ME</g:s>
       </g:gg>
     </g:w>

040. %a MIN<({d}A.NUN.NA)>
     {d}A.NUN.NA
     <g:w form="{d}A.NUN.NA" xml:lang="akk">
       <g:surro>
         <g:s>MIN</g:s>
         <g:gg g:type="group">
           <g:gg g:type="logo">
             <g:d g:pos="pre" g:role="semantic">
               <g:v>d</g:v>
             </g:d>
             <g:s g:delim="." g:logolang="sux" g:role="logo">A</g:s>
             <g:s g:delim="." g:logolang="sux" g:role="logo">NUN</g:s>
             <g:s g:logolang="sux" g:role="logo" g:surroEnd="X000001.1.1.0">NA</g:s>
           </g:gg>
         </g:gg>
       </g:surro>
     </g:w>
     
