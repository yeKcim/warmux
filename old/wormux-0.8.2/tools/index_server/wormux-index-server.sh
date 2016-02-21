#!/bin/sh -e
### BEGIN INIT INFO
# Provides:          wormux-index-server
# Required-Start:    $network
# Required-Stop:
# Default-Start:     S
# Default-Stop:      0 6
# Short-Description: Raise Wormux index server
### END INIT INFO

WMX_SERVER=/usr/bin/wormux_index_server
WMX_RUN_PID=/var/run/wormux_index_server.pid

[ -x $WMX_SERVER ] || exit 0
. /lib/lsb/init-functions

RET=0

wmx_msg() {
  log_daemon_msg "$1ing Wormux index server" "wormux_index_server"
}

[ -r /etc/default/rcS ] && . /etc/default/rcS

case "$1" in
  start)
    rm -f $WMX_RUN_PID
    wmx_msg "Start"
    if start-stop-daemon --start --quiet --pidfile $WMX_RUN_PID \
         --exec $WMX_SERVER ; then
      log_end_msg 0
    else
      log_end_msg 1
      RET=1
    fi
    ;;
  stop)
    wmx_msg "Stopp"
    start-stop-daemon --stop --quiet --signal 1 --pidfile $WMX_RUN_PID
    log_end_msg 0
    ;;
  reload|force-reload)
    wmx_msg "Reload"
    if start-stop-daemon --stop --quiet --signal 1 \
         -pidfile $SMARTDPID; then
      log_end_msg 0
    else
      log_end_msg 1
      RET=1
    fi
    ;;
  restart)
    wmx_msg "Restart"
    start-stop-daemon --stop --quiet --oknodo --retry 30 --pidfile $WMX_RUN_PID
    rm -f $WMX_RUN_PID
    if start-stop-daemon --start --quiet --pidfile $WMX_RUN_PID \
         --exec $WMX_SERVER ; then
      log_end_msg 0
    else
      log_end_msg 1
      RET=1
    fi
    ;;
  *)
     echo "Usage: $0 {start|stop|restart|reload|force-reload}"
     exit 1
esac

exit $RET
