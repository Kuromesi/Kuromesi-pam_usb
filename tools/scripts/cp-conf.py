#!/usr/bin/python

import sys, os
import json
import re

FAPOLICYD_TRUST_PATH = '/etc/fapolicyd/trust.d'
FAPOLICYD_RULES_PATH = '/etc/fapolicyd/rules.d'
PAMUSB_CONF_PATH = '/etc'

def parse_fields(argv):
    recv = sys.stdin.readline()
    with open('/root/recv.txt', 'a') as f:
        f.write(recv + "\n")
    data = {}
    try:
        data = json.loads(recv)
    except exception:
        print "Json parsing failed"
    return data

if __name__ == "__main__":
    val = parse_fields(sys.argv)
    if val:
        src_path = val['parameters']['alert']['syscheck']['path']
        matched = ""
        path = ""
        if re.match('.*fapolicyd/(.*\.rules)$', src_path):
            matched = re.match('.*fapolicyd/(.*\.rules)$', src_path).group(1)
            path = FAPOLICYD_RULES_PATH
        elif re.match('.*fapolicyd/(.*\.trust)$', src_path):
            matched = re.match('.*fapolicyd/(.*\.trust)$', src_path).group(1)
            path = FAPOLICYD_TRUST_PATH
        elif re.match('.*pamusb/(.*\.conf)$', src_path):
            matched = re.match('.*pamusb/(.*\.conf)$', src_path).group(1)
            path = PAMUSB_CONF_PATH
        dest_path = os.path.join(path, matched)
        os.system("echo y | cp %s %s"%(src_path, dest_path))