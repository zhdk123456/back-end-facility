import sys
import time
import socket
import threading

def listen_socket(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((ip, port))
    sock.listen(12) #backlog
    return sock

def open_socket(num, ip, port):

    sockets = []

    for i in range(0, num):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            sock.connect((ip, port))
            sockets.append(sock)
        except:
            print "connect to " + ip + ":%d" % port + " failed."

    return sockets

def operate_socket(sockets, msg, testId):

    for i in range(0, len(sockets)):
        sockets[i].send(msg + ":data from socket client:%d, testID(%d)\0" % (i, testId))

def accept_proc(sock, msg):


    while True:

        try:
            conn, adress = sock.accept()
            print "py client accept one socket!"
            conn.send(msg)

            buf = conn.recv(1024)
            conn.close()

            if buf == msg:
                print "py client recv:" + buf
            else:
                print "rev stop signal from server, stop listening\n"
                return
        except:
            print "exception on recv of accepted socket"
            return

class socket_thread(threading.Thread):

    def __init__(self, proc, arg1, arg2):

        self.proc = proc
        self.arg1  = arg1
        self.arg2  = arg2
        threading.Thread.__init__(self)

    def run(self):
        self.proc(self.arg1, self.arg2)

if __name__ == '__main__':

    ip = sys.argv[1]
    port = int(sys.argv[2])
    listen = int(sys.argv[3])
    id = int(sys.argv[4])

    listen_sock = listen_socket("localhost", listen)
    sockets = open_socket(23, ip, port)

    msg = "py server got you! welcome!(testId:%d)\0" % id

    accept_thread = socket_thread(accept_proc, listen_sock, msg)
    accept_thread.start()

    for i in range(1, 3):
        print "send data for the %d(th) round\n" % i
        operate_socket(sockets, "py test client:127.0.0.1:%d:%d" % (listen, i), id)

    time.sleep(1)

    for i in range(0, len(sockets)):
        buf = sockets[i].recv(1024)
        print "recv:" + buf + "\n"

    sockets[0].send("py test client:127.0.0.1:%d:no connect:testID(%d)\0" % (listen, id));

    accept_thread.join()

    print "exiting, close all socket connections\n"

    for i in range(0, len(sockets)):
        sockets[i].close()

    listen_sock.close()

