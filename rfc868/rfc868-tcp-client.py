import socket
import datetime


# List of servers
# http://tf.nist.gov/tf-cgi/servers.cgi

"""
When used via TCP the time service works as follows:
   S: Listen on port 37 (45 octal).
   U: Connect to port 37.
   S: Send the time as a 32 bit binary number.
   U: Receive the time.
   U: Close the connection.
   S: Close the connection.
"""

#IP = 'time-d.nist.gov'
IP = 'localhost'

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((IP, 37))
data = s.recv(4)
s.close()

rfc868time = ord(data[0]) << 24 | ord(data[1]) << 16 | ord(data[2]) << 8 | ord(data[3])
unixtime = rfc868time - 2208988800
timestring = datetime.datetime.fromtimestamp(unixtime).strftime('%Y-%m-%d %H:%M:%S')

print "number of seconds since 00:00 (midnight) 1 January, 1900 GMT:", rfc868time
print "unixtime:", unixtime
print "human readable:", timestring



