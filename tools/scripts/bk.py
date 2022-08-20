#!/usr/bin/python
# -*- coding: UTF-8 -*-

import MySQLdb
import sys
import json

def parse_fields(argv):
    recv = sys.stdin.readline()
    with open('/root/recv.txt', 'a') as f:
        f.write(recv)
    data = {}
    try:
        data = json.loads(recv)
    except exception:
        print "Json parsing failed"
    try:
        print data['parameters']['alert']['data']
        return data['parameters']['alert']['data']
    except Exception, e:
        print e
        print "No such fields"
    return {}

def update_status(user, ip):
    db = MySQLdb.connect("192.168.135.145", "root", "test207", "wazuh")
    cursor = db.cursor()
    sql = "INSERT INTO user_status(username, ip) VALUES ('%s', '%s')"%(user, ip)

    try:
        print "Executing: %s"%sql
        cursor.execute(sql)
        db.commit()
        print "Query succeded."
    except Exception, e:
        print e
        db.rollback()
        print "Query failed."
    db.close()

if __name__ == "__main__":
    val = parse_fields(sys.argv)
    user = val['active_user']
    ip = val['active_ip']
    update_status(user, ip)