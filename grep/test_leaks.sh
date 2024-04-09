#!/bin/bash

LEAK_COUNTER=0
COUNTER=0

declare -a multy_testing=(
"VAR"
"VAR test1.txt"
"test127.txt VAR"
"test.txt"
"D VAR test1.txt"
"S VAR test1.txt test2.txt test1.txt"
"-e s VAR -o test3.txt"
"-e ^int VAR test1.txt test2.txt test1.txt"
"-f pattern_file.txt test3.txt test2.txt test1.txt"
"-f pattern_file.txt test3.txt test2.txt test6.txt"
)

declare -a unique_testing=(
"abc no_file.txt"
"abc -f no_file -ivclnhso no_file.txt"
"-e S -i -nh test2.txt test1.txt test2.txt test1.txt"
"-e char -v test2.txt test1.txt"
"-e int grep.c"
)

vg_checking () {
  t=$(echo $@ | sed "s/VAR/$var/")
  #linux
  # valgrind --tool=memcheck --leak-check=full --track-origins=yes --show-reachable=yes --num-callers=20 --track-fds=yes --log-file="vg_info.log" ./s21_grep $t > vg_out.log
  
  #ios
  leaks -atExit -- ./s21_grep $t > vg_out.log
  VG_RES="$(grep LEAK -c vg_out.log)"

  #linux
  # VG_RES="$(grep LEAK -c vg_info.log)"
  (( COUNTER++ ))
  if [ "$VG_RES" == "0" ]
    then
        echo "$COUNTER: NO LEAKS"
        echo "$t"
    else
      (( LEAK_COUNTER++ ))
        echo "$COUNTER: THERE ARE LEAKS"
        exit 1
  fi
  rm vg_out.log
  #  linux
  # rm vg_info.log vg_out.log
}


for var1 in e i v c l n h s o
do
  for i in "${multy_testing[@]}"
  do
    var="-$var1"
    vg_checking $i
  done
done

for var1 in e i v c l n h s o
do
  for var2 in e i v c l n h s o
  do
    for i in "${multy_testing[@]}"
      do
        var="-$var1$var2"
        vg_checking $i
      done
  done
done

for i in "${unique_testing[@]}"
do
    var="-"
    vg_checking $i
done

echo "LEAKS: $LEAK_COUNTER"
echo "ALL: $COUNTER"