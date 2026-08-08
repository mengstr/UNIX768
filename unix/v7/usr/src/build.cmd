set -e

cd cmd

for prog in cron atrun at dmesg wall diffh diff3 units ac accton sa
do
	echo CC $prog.c
	cc -O2 -s -o $prog $prog.c
done

for prog in wc uniq tr touch tail sum sort sleep rev cmp join basename
do
	echo CC $prog.c
	cc -O2 -s -o $prog $prog.c
done

for prog in id tty kill mesg split time nice cal yes ed comm tee random crypt makekey
do
	echo CC $prog.c
	cc -O2 -s -o $prog $prog.c
done

for prog in stty who write passwd su newgrp grep fgrep tsort test find look
do
	echo CC $prog.c
	cc -O2 -s -o $prog $prog.c
done

for prog in pr diff file col ptx
do
	echo CC $prog.c
	cc -O2 -s -o $prog $prog.c
done

echo YACC egrep.y
yacc egrep.y
mv y.tab.c egrep.c
echo CC egrep.c
cc -s -o egrep egrep.c

echo CC m68kspeed
AS=${AS-/usr/lib/ack/as}
$AS -o m68kasm.o m68kspeed.s
cc -O2 -c -o m68kmain.o m68kspeed.c
cc -s -o m68kspeed m68kmain.o m68kasm.o

cd ../ports/xargs
echo CC xargs.c
cc -O2 -s -o xargs xargs.c

cd ../../cmd/sed
sh build

cd ../sh
sh build
exit 0
