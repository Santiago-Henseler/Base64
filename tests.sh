#!/bin/bash

total_passed=0
total_failed=0

passed_test() {
	echo -e "\e[32m[PASSED] $1\e[0m" 
        total_passed=$((total_passed+1))
}

failed_test(){
        echo -e "\e[31m[FAILED] $1\e[0m"
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
	if [[ $( echo -n $( echo -n "Man" | ./base64 -e ) | ./base64 -d ) -eq "Man" ]]; then 
		passed_test "Test: Encode-Decode Man"	
	else 	
		failed_test "Test: Encode-Decode Man" 
	fi
}


test_encode_foo(){
        if [[ $( echo -n "foo" | ./base64 -e ) -eq "Zm9v" ]]; then
                passed_test "Test: Encode Foo"
        else
                failed_test "Test: Encode Foo"
        fi
}

test_decode_foo(){
        if [[ $( echo -n "Zm9v" | ./base64 -d ) -eq "foo" ]]; then
                passed_test "Test: Decode Foo"
        else
                failed_test "Test: Decode Foo"
        fi
}

test_encode_decode_foo(){
        if [[ $( echo -n $( echo -n "foo" | ./base64 -e ) | ./base64 -d ) == "foo" ]]; then
                passed_test "Test: Encode-Decode foo"
        else
                failed_test "Test: Encode-Decode foo"
        fi
}

test_zeros(){
	if [[ $( head -c 1 /dev/zero | ./base64 -e ) == "AA==" ]]; then
		if [[ $( head -c 2 /dev/zero | ./base64 -e ) == "AAA=" ]]; then
			if [[ $( head -c 3 /dev/zero | ./base64 -e ) == "AAAA" ]]; then 
				passed_test "Test: Input zeros" 
			else
				failed_test "Test: Input zeros"
			fi
		else
			failed_test "Test: Input zeros"
		fi 
	else 
		failed_test "Test: Input zeros"
	fi
}

test_newline(){
	if [[ $( echo | ./base64 -e ) == "Cg==" ]]; then 
		passed_test "Test: New line"
	else
		failed_test "Test: New line"
	fi
}

test_big_read_from_random(){
	echo -e "\e[36mRunning test big read from random (takes time...)\e[0m"
	head -c 104857600 /dev/zero | ./base64 -e > /dev/null
	if [ $? -eq 0 ]; then 
		passed_test "Test: Big read from random"
	else
		failed_test "Test: Big read from random"
	fi 
}

test_encode_Man
test_decode_Man
test_encode_decode_Man
test_encode_foo
test_decode_foo
test_encode_decode_foo
test_zeros
test_newline
test_big_read_from_random

echo "Passed: $total_passed of $((total_passed + total_failed))"
