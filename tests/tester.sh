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

declare -a lstopt=("" "-n" "-E" "-n -E")

echo ""
echo "===================================================="
echo " Comparing own version of cat with system's cat"
echo "===================================================="

for options in "${lstopt[@]}"
do
	for entry in $SCRIPTPATH/testing-files/*
	do
		echo " Comparing: ./cat $options $entry "
		
		# Executes system's cat
		cat $options $entry > $SCRIPTPATH/system_output
		
		# Executes own version of cat
		$SCRIPTPATH/../cat $options $entry > $SCRIPTPATH/my_output
		
		DIFF=$(diff -u $SCRIPTPATH/system_output $SCRIPTPATH/my_output)

		if [ "$DIFF" != "" ]
		then
		    echo " Result: Differences were found!"
		else
			echo " Result: Successfull!"
		fi

		rm $SCRIPTPATH/system_output $SCRIPTPATH/my_output
	done
done

