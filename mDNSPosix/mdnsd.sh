#!/bin/sh
#
# Linux /etc/init.d script to start/stop the mdnsd daemon.

DAEMON=/usr/sbin/mdnsd

test -f $DAEMON || exit 0

# Some systems have start-stop-daemon, some don't. 
if [ -e /sbin/start-stop-daemon ]; then
	START=start-stop-daemon --start --quiet --exec
	STOP=start-stop-daemon --stop -s TERM --quiet --oknodo --exec
else
	# Source function library.
	if [ -e /etc/init.d/functions ]; then 
		. /etc/init.d/functions
	else
		. /etc/rc.d/init.d/functions
	fi
	killprocterm() {
		killproc $1 -TERM
	}
	START=
	STOP=killprocterm
fi

case "$1" in
    start)
	echo -n "Starting mDNS daemon:"
	echo -n " mdnsd"
        $START $DAEMON
	echo "."
	;;
    stop)
        echo -n "Stopping mDNS daemon:"
        echo -n " mdnsd" ; $STOP $DAEMON
        echo "."
	;;
    reload|restart|force-reload)
		echo -n "Restarting mDNS daemon:"
		$STOP $DAEMON
		sleep 1
		$START $DAEMON
		echo -n " mdnsd"
	;;
    *)
	echo "Usage: /etc/init.d/mDNS {start|stop|reload|restart}"
	exit 1
	;;
esac

exit 0

