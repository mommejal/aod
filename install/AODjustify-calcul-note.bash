#!/bin/bash

ORIG=/matieres/4MMAOD6/Benchmark/
VERIF=/matieres/4MMAOD6/install/


function testAOD {
    cp $ORIG/$1.in AODTESTS/$1.in
    ./TPAODjustify/bin/AODjustify $2 AODTESTS/$1 
    $VERIF/AODjustify-verify $2  $ORIG/$1.in AODTESTS/$1.out 
    local status=$?
    if [ $status -ne 0 ]; then
        echo "??? TEST ECHOUE sur test $1" 1>&2
    else
        note=$(($note+1)) 
    fi
    return $status
}

function testAODerreur {
    cp $ORIG/$1.in AODTESTS/$1.in
    ./TPAODjustify/bin/AODjustify $2 AODTESTS/$1  
    local status=$?
    if [ $status -ne 0 ]; then
        note=$(($note+1)) 
    else
        echo "??? TEST d'erreur ECHOUE sur test $1" 1>&2
    fi
    return $status
}

function testvalgrind  {
    echo " .... TEST valgrind $1 " 1>&2
    cp $ORIG/$1.in AODTESTS/$1.in
    echo "valgrind  --leak-check=full  ./TPAODjustify/bin/AODjustify $2 AODTESTS/$1 "
    valgrind  --leak-check=full  ./TPAODjustify/bin/AODjustify $2 AODTESTS/$1 2>&1 | grep "== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)" > valgrind.status
    if [ -s valgrind.status ]; then
        note=$(($note+1)) 
    else
        echo "??? TEST valgind  ECHOUE sur test $1" 1>&2
    fi
    rm valgrind.status
}


\rm -R AODTESTS
mkdir AODTESTS
note=0

testAOD court-un-paragraphe 20
testAOD court-plusieurs-paragraphes-ISO-8859-1 40
testAOD foo2.iso8859-1 29
testAODerreur toto 40
testAODerreur foo 0
testAOD longtempsjemesuis.ISO-8859 60
testAOD ALaRechercheDuTempsPerdu.ISO-8859-1.M80 80
testAOD ALaRechercheDuTempsPerdu-1paragraphe-ISO-8859-1 100
testAOD ALaRechercheDuTempsPerdu-1paragraphe-ISO-8859-1 1000
if [ $note -eq 9 ]; then   # Bonus si tous les tests sont corrects et la mémoire bien allouée et libérée
   testvalgrind longtempsjemesuis.ISO-8859 80 
fi
echo "********** note= $note/10" 2>&1
return $note
