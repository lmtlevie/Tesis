import select
import socket
import sys
import time
import matplotlib.pyplot as plt
import numpy as np
from matplotlib import pyplot as plt
from collections import deque

# UDP

# class that holds analog data for N samples
class AnalogData:
  # constr
  def __init__(self, maxLen):
    self.ay = deque([0.0]*maxLen)
    self.ax = deque([0.0]*maxLen)
    self.maxLen = maxLen

  # ring buffer
  def addToBuf(self, buf, val):
    if len(buf) < self.maxLen:
      buf.append(val)
    else:
      buf.pop()
      buf.appendleft(val)

  # add data
  def add(self, dataY, dataX):
    self.addToBuf(self.ay, dataY)
    self.addToBuf(self.ax, dataX)
    
# main() function
def main():

    # expects 1 arg - udp port string
    if(len(sys.argv) != 2):
        print 'Example usage: python serverUDP-PD.py "udp-port"'
        exit(1)

    port = int(sys.argv[1])

    try:
        # create a socket
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print "Socket created"
    except socket.error, msg :
        print 'Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        sys.exit()

    try:
        # set non blocking socket
        server_socket.setblocking(False)
    except socket.error , msg:
        print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        sys.exit()

    try:
        # bind the socket to the port
        server_socket.bind(("", port))
    except socket.error , msg:
        print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
        sys.exit()

    print 'Socket bind complete'
    print 'Listening on port ' + str(port)

    read_list = [server_socket]

    # plot parameters
    analogData1 = AnalogData(100)
    analogData2 = AnalogData(100)
    # analogPlot = AnalogPlot(analogData)

    # set plot to animated
    fig1 = plt.figure()
    plt.ion() # habilita que se pueda redibujar
    # handler, = plt.plot(analogData.ay) # plt.plot(data) asume que Y = data y X son indices entre 0 y len(data)
    handler1, = plt.plot(analogData1.ax,analogData1.ay)
    # para dejar linda la ventana
    plt.ylim([-10, 10])
    plt.grid(True)
    plt.title("Received")
    plt.xlabel("t")
    plt.ylabel("y")
    plt.legend("data")
    # sin las dos lineas que siguen no abre la ventana del plot
    fig1.canvas.draw()
    plt.show(False)

    # set plot to animated
    fig2 = plt.figure()
    plt.ion() # habilita que se pueda redibujar
    # handler, = plt.plot(analogData.ay) # plt.plot(data) asume que Y = data y X son indices entre 0 y len(data)
    handler2, = plt.plot(analogData2.ax,analogData2.ay)
    # para dejar linda la ventana
    plt.ylim([-10, 10])
    plt.grid(True)
    plt.title("Received with local time")
    plt.xlabel("t")
    plt.ylabel("y")
    plt.legend("data")
    # sin las dos lineas que siguen no abre la ventana del plot
    fig2.canvas.draw()
    plt.show(False)

    print 'plotting data...'

    flag = 1 # first package received

    try:
        while True:
            readable, writable, errored = select.select(read_list, [], [],0.001) # if not set timeout = inf
            for s in readable: # elementos de la lista read_list que esten disponibles para ser leidos => s es de tipo socket
                if flag:
                    # signal parameteres
                    t0 = time.time()
                    flag = 0
                # data is read from the connection with recvfrom(size), size: number of characters 
                data, addr = s.recvfrom(18) # UDP
                print "data: ", data
                if data != "quit": # si recibe un dato lo reenvia para mantener vivo al socket
                    # data is transmitted with sendto()
                    # sending data back to the client
                    s.sendto(str(data),addr) # UDP
                    print 'Message[' + addr[0] + ':' + str(addr[1]) + '] - ' + data.strip() # data.strip() se queda con la cadena recibida dentro de los 1024 caracteres del buffer.
                    y = data.split()[0]
                    t = data.split()[1]
                    localt = time.time() - t0
                    analogData1.add(float(y),float(t))
                    analogData2.add(float(y),localt)
                    # analogPlot.update(analogData)

                    # update plot 1
                    handler1.set_ydata(analogData1.ay)
                    handler1.set_xdata(analogData1.ax)
                    plt.figure(fig1.number)
                    plt.xlim([min(analogData1.ax),max(analogData1.ax)])
                    fig1.canvas.draw()
                    # update plot 2
                    handler2.set_ydata(analogData2.ay)
                    handler2.set_xdata(analogData2.ax)
                    plt.figure(fig2.number)
                    plt.xlim([min(analogData2.ax),max(analogData2.ax)])
                    fig2.canvas.draw()

                else: # si recibe 0 el cliente cerro el socket
                    print "Remove connection from", s.getpeername()
                    s.close()
    except KeyboardInterrupt:
        print 'Closing server... \n'

#    finally:
#        for s in read_list:
#            s.close()
#            read_list.remove(s)

# call main
if __name__ == '__main__':
    main()
