#!/bin/bash

submission=$1
mkdir ./test_dir

echo $submission

# Extract the Entry number
entryNoRegex="[0-9]{4}[A-Z]{2}.[0-9]{4}"
if [[ $submission =~ $entryNoRegex ]]; then
	entryNum=${BASH_REMATCH[0]}
	echo "Entry Number: $entryNum"
fi

echo "Setting the test directory"

tar -xzvf "$submission" -C ./test_dir
cp testcase_*.c *.sh ./test_dir
cd ./test_dir

FILE=*[Rr][Ee][Pp][Oo][Rr][Tt]*.pdf
if ! ls $FILE 1>/dev/null 2>&1; then
	echo "Report does not exist"
fi

adduprogs() {
	# takes the program as argument
	echo "Modifying the Makefile"
	EXTRAUPROGS="UPROGS="
	# For all testcases
	EXTRAUPROGS="$EXTRAUPROGS $1"

	# remove assig2_* files from UPROGS if already present
	sed -i 's/_testcase_.//g' Makefile
	sed -i 's/UPROGS=/'"$EXTRAUPROGS"'/g' Makefile
	make fs.img
	make
}

sed -i 's/_testcase_.//g' Makefile
# Add the test cases to the Makefile

# Change the gcc binary
sed -i 's/gcc-100\|gcc-9/gcc/g' Makefile

echo "Executing the test cases"

pkill qemu-system-x86
pkill qemu-system-i386
make clean

# Executing each test case independently with a timeout of 100 seconds
echo "Running..1"
python3 gen_exploit.py 4
echo 0 >aslr_flag
adduprogs _testcase_1
timeout 30s ./test_assig3.sh testcase_1 | grep -i 'SECRET_STRING:' | sed 's/$ //g' >res_assig3_1
echo 1 >aslr_flag
timeout 30s ./test_assig3.sh testcase_1 | grep -i 'SECRET_STRING:' | sed 's/$ //g' >res_assig3_1_aslr

# echo "Running..2"
# adduprogs _assig3_2
# python3 gen_exploit.py 21
# timeout 30s ./test_assig3.sh testcase_2 |grep -i 'SECRET:'|sed 's/$ //g' > res_assig3_2
# echo 1 >> aslr_flag
# timeout 30s ./test_assig3.sh testcase_2 |grep -i 'SECRET:'|sed 's/$ //g' > res_assig3_2_aslr

# echo "Running..3"
# adduprogs _assig3_3
# python3 gen_exploit.py 65
# timeout 30s ./test_assig3.sh testcase_3 |grep -i 'SECRET:'|sed 's/$ //g' > res_assig3_3
# echo 1 >> aslr_flag
# timeout 30s ./test_assig3.sh testcase_3 |grep -i 'SECRET:'|sed 's/$ //g' > res_assig3_3_aslr

check_test=1
total_test=0

echo "" >.output
marks="$entryNum"

for ((t = 1; t <= $check_test; ++t)); do
	echo -n "Test #${t}: "

	# NOTE: we are doing case insensitive matching.  If this is not what you want,
	# just remove the "-i" flag
	# Ignoring blank spaces

	file_name="res_assig3_$t"
	aslr_file_name="res_assig3_${t}_aslr"

	if [ -s "$file_name" ]; then
		echo $file_name
		echo $aslr_file_name
		if [! -s "$aslr_file_name" ]; then
			# echo $aslr_file_name
			echo -e "PASS"
			marks="$marks,1" # 1 mark for pass
			((total_test++))
		else
			echo -e "FAIL2"
		fi
	else
		echo -e "FAIL"
		marks="$marks,0" # 0 mark for fail
		echo "Output for test case $t:" >>.output
		cat res_assig3_$t >>.output
		echo "Output for test case $t: after turning on aslr " >>.output
		cat res_assig3_$t\_aslr >>.output
	fi
done
