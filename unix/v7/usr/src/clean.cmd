set -e

cd cmd

for prog in cron atrun at dmesg wall diffh diff3 units ac accton sa
do
	rm -f $prog
done

for prog in wc uniq tr touch tail sum sort sleep rev cmp join basename
do
	rm -f $prog
done

for prog in id tty kill mesg split time nice cal yes ed comm tee random crypt makekey
do
	rm -f $prog
done

for prog in stty who write passwd su newgrp grep fgrep egrep
do
	rm -f $prog
done

for prog in tsort test find look pr diff file col ptx
do
	rm -f $prog
done

rm -f egrep.c y.tab.c m68kspeed m68kasm.o m68kmain.o
rm -f *.o

cd sed
sh clean
cd ../sh
sh clean
cd ../../ports/xargs
rm -f xargs *.o
exit 0
