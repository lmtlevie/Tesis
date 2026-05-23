import socket  
import sys
from time import sleep
import numpy as np

# UDP 

# expects 1 arg - udp port string
if(len(sys.argv) != 3):
    print 'Example usage: python clientUDP-PD.py "ip" "udp-port"'
    exit(1)

port = int(sys.argv[2])
ip = sys.argv[1]

# create datagram udp socket
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
except socket.error:
    print 'Failed to create socket'
    sys.exit()

print 'Talking to port ' + str(port)

# signal parameteres
t = 0
f = 0.1
step = 0.1

try:
    while True:  
        try:
            x = np.sin(2*np.pi*f*t)
            s.sendto(str(x),(ip,port)) # UDP
            print "Message sent: x = " + str(x) + " t = " + str(t)
            sleep(step)
            t = t + step

        except socket.error, msg:
            print 'Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
            s.close()
            sys.exit()

except KeyboardInterrupt:
    print 'Closing client... \n'

s.close()
