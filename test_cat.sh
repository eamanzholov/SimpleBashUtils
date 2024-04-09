#!/bin/bash

SUCCESS=0
FAIL=0
COUNT=0
DIFFERENCE=""

declare -a tests=(
"VAR test127.txt"
)

declare -a extra=(
"-s test1.txt"
"-b -e -n -s -t -v test2.txt"
"-benstv test1.txt"
"-bn test3.txt"
"-t test3.txt"
"-n test2.txt"
"-n -b test1.txt"
"-s -n -e test4.txt"
"-n test2.txt test1.txt"
"-n test1.txt"
"-n test1.txt test2.txt"
"-v test5.txt"
"abc no_file.txt"
# "-E test127.txt"
# "--number-nonblank test2.txt"
# " --number test5.txt"
# "--squeeze-blank test3.txt test5.txt"
# "-T test1.txt test127.txt"
)

testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./s21_cat $t > test_s21_cat.log
    cat $t > test_sys_cat.log
    DIFFERENCE="$(diff -s test_s21_cat.log test_sys_cat.log)"
    (( COUNT++ ))
    if [ "$DIFFERENCE" == "Files test_s21_cat.log and test_sys_cat.log are identical" ]
    then
        (( SUCCESS++ ))
        echo "$COUNT: SUCCESS"
    else
        (( FAIL++ ))
        echo "$COUNT: FAIL: cat $t"
    fi
    rm test_s21_cat.log test_sys_cat.log
}

for i in "${extra[@]}"
do
    var="-"
    testing $i
done

for var1 in b e n s t v
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

for var1 in b e n s t v
do
    for var2 in b e n s t v
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                testing $i
            done
        fi
    done
done

for var1 in b e n s t v
do
    for var2 in b e n s t v
    do
        for var3 in b e n s t v
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    testing $i
                done
            fi
        done
    done
done


for var1 in b e n s t v
do
    for var2 in b e n s t v
    do
        for var3 in b e n s t v
        do
            for var4 in b e n s t v
            do
                if [ $var1 != $var2 ] && [ $var2 != $var3 ] \
                && [ $var1 != $var3 ] && [ $var1 != $var4 ] \
                && [ $var2 != $var4 ] && [ $var3 != $var4 ]
                then
                    for i in "${tests[@]}"
                    do
                        var="-$var1 -$var2 -$var3 -$var4"
                        testing $i
                    done
                fi
            done
        done
    done
done

echo "TOTAL FAILS: $FAIL"
echo "TOTAL SUCCESSES: $SUCCESS"
echo "ALL: $COUNT"
