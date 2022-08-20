from dataclasses import fields
import os
import socket
import syslog


class Log:
    def __init__(self):
        syslog.openlog('Active-User', syslog.LOG_PID | syslog.LOG_PERROR,
                       syslog.LOG_AUTH)

    def info(self, message):
        self.__logMessage(syslog.LOG_NOTICE, message)

    def error(self, message):
        self.__logMessage(syslog.LOG_ERR, message)

    def __logMessage(self, priority, message):
        syslog.syslog(priority, message)


s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect(("192.168.135.1", 80))
ip = s.getsockname()[0]
process = os.popen('w')
output = process.readlines()
process.close()
users = set()
for line in output[2:]:
    temp = line.split(" ")
    if temp[0] == "root":
        continue
    users.add((temp[0], ip))
print users
logger = Log()
for user, addr in users:
    logger.info("User: %s is active on IP: %s" % (user, addr))
    