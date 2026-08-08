cp cmd/cron cmd/dmesg cmd/wall cmd/accton $1/etc
chmod 0755 $1/etc/cron $1/etc/dmesg $1/etc/wall $1/etc/accton
cp cmd/atrun cmd/diffh cmd/diff3 $1/usr/lib
chmod 0755 $1/usr/lib/atrun $1/usr/lib/diffh $1/usr/lib/diff3
cp cmd/at cmd/ac cmd/sa cmd/units $1/bin
cp cmd/wc cmd/uniq cmd/tr cmd/touch cmd/tail cmd/sum $1/bin
cp cmd/sort cmd/sleep cmd/rev cmd/cmp cmd/join cmd/basename $1/bin
cp cmd/id cmd/tty cmd/kill cmd/mesg cmd/split cmd/time $1/bin
cp cmd/nice cmd/cal cmd/yes cmd/ed cmd/comm cmd/tee cmd/random cmd/crypt $1/bin
cp cmd/stty cmd/who cmd/write cmd/passwd cmd/su cmd/newgrp $1/bin
cp cmd/grep cmd/fgrep cmd/egrep cmd/tsort cmd/test cmd/find $1/bin
cp cmd/look cmd/pr cmd/diff cmd/file cmd/col cmd/ptx $1/bin
chmod 0755 $1/bin/at $1/bin/ac $1/bin/sa $1/bin/units
chmod 0755 $1/bin/wc $1/bin/uniq $1/bin/tr $1/bin/touch
chmod 0755 $1/bin/tail $1/bin/sum $1/bin/sort $1/bin/sleep
chmod 0755 $1/bin/rev $1/bin/cmp $1/bin/join $1/bin/basename
chmod 0755 $1/bin/id $1/bin/tty $1/bin/kill $1/bin/mesg
chmod 0755 $1/bin/split $1/bin/time $1/bin/nice $1/bin/cal
chmod 0755 $1/bin/yes $1/bin/ed $1/bin/comm $1/bin/tee
chmod 0755 $1/bin/random $1/bin/crypt
cp cmd/makekey $1/usr/lib/makekey
chmod 0755 $1/usr/lib/makekey
chmod 0755 $1/bin/stty $1/bin/who $1/bin/write $1/bin/passwd
chmod 0755 $1/bin/su $1/bin/newgrp $1/bin/grep $1/bin/fgrep
chmod 0755 $1/bin/egrep $1/bin/tsort $1/bin/test $1/bin/find
chmod 0755 $1/bin/look $1/bin/pr $1/bin/diff $1/bin/file
chmod 0755 $1/bin/col $1/bin/ptx
cp ports/xargs/xargs $1/local/bin/xargs
cp cmd/m68kspeed $1/local/bin/m68kspeed
chmod 0755 $1/local/bin/xargs $1/local/bin/m68kspeed
cp cmd/sed/sed $1/bin/sed
chmod 0755 $1/bin/sed
cp cmd/sh/sh $1/bin/sh.new
chmod 0755 $1/bin/sh.new
ln $1/bin/sh $1/tmp/sh.$$
mv $1/bin/sh.new $1/bin/sh
rm -f $1/tmp/sh.$$
cp rc.stage3 $1/etc/rc
cp crontab.stage3 $1/usr/lib/crontab
cp units $1/usr/lib/units
cp wtmp $1/usr/adm/msgbuf
cp wtmp $1/usr/adm/wtmp
cp wtmp $1/usr/adm/acct
chmod 0755 $1/etc/rc
cp bin/diff3 bin/lorder bin/nohup bin/true bin/false $1/bin
chmod 0755 $1/bin/diff3 $1/bin/lorder $1/bin/nohup
chmod 0755 $1/bin/true $1/bin/false
echo install.cmd: complete
