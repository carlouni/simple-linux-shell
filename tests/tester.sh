#!/bin/bash
####################################################
# cat tester shell script
# Compares system cat output with own version of cat
# for testing files found in testing-files directory
####################################################
SECONDS=0

# Getting scripts path
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")

declare -a lstopt=("n" "E" "b" "s" "v" "t" "nE" "nb" "ns" "nv" "nt" "ne" "Eb" "Es" "Ev" "Et" "Ee" "bs" "bv" "bt" "be" "sv" "st" "se" "vt" "ve" "te" "nEb" "nEs" "nEv" "nEt" "nEe" "nbs" "nbv" "nbt" "nbe" "nsv" "nst" "nse" "nvt" "nve" "nte" "nEbs" "nEbv" "nEbt" "nEbe" "nEbsv" "nEbst" "nEbse" "nEbsvt" "nEbsve" "nEbsvte")

echo ""
echo "===================================================="
echo " Comparing own version of cat with system's cat"
echo "===================================================="

for options in "${lstopt[@]}"
do
	for entry in $SCRIPTPATH/testing-files/*
	do
		echo " Comparing: ./cat -$options $entry "
		
		# Executes system's cat
		cat "-$options" $entry > $SCRIPTPATH/system_output
		
		# Executes own version of cat
		$SCRIPTPATH/../cat "-$options" $entry > $SCRIPTPATH/my_output
		
		DIFF=$(diff -u $SCRIPTPATH/system_output $SCRIPTPATH/my_output)

		if [ "$DIFF" != "" ]
		then
		    echo " Result: Differences were found!"
                    echo "$DIFF"
		else
			echo " Result: Successfull!"
		fi

		rm $SCRIPTPATH/system_output $SCRIPTPATH/my_output
	done
done

