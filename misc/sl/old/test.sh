#!/bin/sh

function t {
    tst=$1
    exp=$2
    got=`tlitsig.plx $tst`
    if [[ $got == *"=> $exp" ]]; then
	echo got $exp for $tst as expected
    else
	echo failed on $tst \(got $got not $exp\)
    fi
}

t du o0000149
t 'lah₄(|DU.DU|)' o0000149.o0000149
t '|URU×(MIN.GA)|' o0002546
t ad₅ o0000599.o0000214
t {ŋeš}ad₅ o0000217.o0000599.o0000214
t 'kiši₁₆(|U₂.GIR₂@g|)' o0000599.o0000214
t A-NUMUN₂ o0000087.o0002666.o0000306
t '|NUN.LAGAR×SAL|' o0002129
t A-KU₄ o0000087.o0000297
t '|(4×ZA)×KUR|' o0000619
t 'ŋeštae' o0000619
t '{tumu}mir-ra{+tu-mu-{d}mi-ra}' o0000254.o0000162.o0000510.o0000556.o0000457.o0000099.o0000455.o0000510
t '{tumu}mir-ra{tu-mu-{d}mi-ra}' o0000254.o0000162.o0000510.o0000556.o0000457.o0000099.o0000455.o0000510
