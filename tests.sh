#!/bin/bash

total_passed=0
total_failed=0

passed_test() {
	echo -e "\e[32m[PASSED] $1\e[0m" 
        total_passed=$((total_passed+1))
}

failed_test(){
        echo -e "\e[31m[FAILED] $1\3[0m"
        total_failed=$((total_failed+1))
}
test_encode_Man(){
	if [[ $( echo -n "Man" | ./base64 -e ) -eq "TWFu" ]]; then
		passed_test "Test: Encode Man"
	else
		failed_test "Test: Encode Man"
	fi
}

test_decode_Man(){
	if [[ $( echo -n "TWFu" | ./base64 -d ) -eq "Man" ]]; then 
		echo -e "\e[32m[PASSED] Test: Decode Man\e[0m"
		total_passed=$((total_passed+1))
	else
		echo -e "\e[31m[FAILED] Test: Decode Man\3[0m"
		total_failed=$((total_failed+1))
	fi	
}

test_encode_decode_Man(){
	echo -n "Man" | ./base64 -e -o .test_encode_decode.tmp
	if [[ $( ./base64 -d -i .test_encode_decode.tmp ) -eq "Man" ]]; then 
		passed_test "Test: Encode-Decode Man"	
	else 	
		failed_test "Test: Encode-Decode Man" 
	fi
}

test_encode_Man
test_decode_Man
test_encode_decode_Man

echo "Passed: $total_passed of $((total_passed + total_failed))"
