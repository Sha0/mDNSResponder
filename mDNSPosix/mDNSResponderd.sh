#!/bin/sh
#
# Linux /etc/init.d script to start/stop the mDNSResponderd daemon.

DAEMON=/usr/sbin/mDNSResponderd

test -f $DAEMON || exit 0

# Some systems have start-stop-daemon, some don't. 
if [ -e /sbin/start-stop-daemon ]; then
	START=start-stop-daemon --start --quiet --exec
	STOP=start-stop-daemon --stop -s TERM --quiet --oknodo --exec
else
	START=
	STOP=killproc -TERM
fi

case "$1" in
    start)
	echo -n "Starting mDNS daemon:"
	echo -n " mDNSResponderd"
        $START $DAEMON
	echo "."
	;;
    stop)
        echo -n "Stopping mDNS daemon:"
        echo -n " mDNSResponderd" ; $STOP $DAEMON
        echo "."
	;;
    reload|restart|force-reload)
		echo -n "Restarting mDNS daemon:"
		$STOP $DAEMON
		sleep 1
		$START $DAEMON
		echo -n " mDNSResponderd"
	;;
    *)
	echo "Usage: /etc/init.d/mDNS {start|stop|reload|restart}"
	exit 1
	;;
esac

exit 0

