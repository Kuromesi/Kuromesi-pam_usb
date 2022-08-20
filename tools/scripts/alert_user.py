#!/usr/bin/python

import sys, os
import json
import re

def parse_fields(argv):
    recv = sys.stdin.readline()
    with open('/root/recv.txt', 'a') as f:
        f.write(recv)
    data = {}
    try:
        data = json.loads(recv)
    except exception:
        print "Json parsing failed"
    return data

if __name__ == "__main__":
    val = parse_fields(sys.argv)
    uid = val['parameters']['alert']['data']['auid']
    result = os.popen('getent passwd %s'%uid)
    result = result.readline()
    user = result.split(":")[0]
    os.system("sudo -u %s DISPLAY=:0 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/%s/bus notify-send 'ALERT' 'YOUR ACTIONS ARE BEING MONITORED'"%(user, uid))