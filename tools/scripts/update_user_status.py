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
        return data
    except Exception, e:
        print e
    return {}

def update_status(users, ip):
    db = MySQLdb.connect("192.168.135.145", "root", "test207", "wazuh")
    cursor = db.cursor()
    sql = "SELECT * FROM user_status WHERE ip='%s'"%ip
    try:
        cursor.execute(sql)
        results = cursor.fetchall()
        for row in results:
            name = row[0]
            if name in users:
                users.remove(name)
            else:
                sql = "DELETE FROM user_status WHERE username='%s' AND ip='%s'"%(name, ip)
                try:
                    cursor.execute(sql)
                    db.commit()
                except:
                    db.rollback()
                    print "Deletion failed"
    except Exception, e:
        print e
        print "Error: unable to fetch data"

    if users:
        for user in users:
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
    try:
        users = val['parameters']['alert']['data']['active_user']
        users = users.split(" ")
        ip = val['parameters']['alert']['data']['active_ip']
        update_status(users, ip)
    except Exception, e:
        print e