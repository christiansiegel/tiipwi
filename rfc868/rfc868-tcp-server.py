import socket
import time

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

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind(('127.0.0.1', 37))
s.listen(1)

while 1:
    print "waiting for connection..."
    conn, addr = s.accept()
    print 'Connection address:', addr

    unixtime = int(time.time())
    rfc868time = unixtime + 2208988800
    
    t = rfc868time
    data = chr((t & 0xff000000)>>24) + chr((t & 0xff0000)>>16) + chr((t & 0xff00)>>8) + chr(t & 0xff) 
    conn.send(data)  # echo
conn.close()



# if you get error 98: use `lsof -i :37` in command line
