'''
goal: a simple udp server to test my udp client.

details: udp client will be report snr and other data every second or so. 
data will be random for now.
after 20 seconds, the udp server will send "terminate" command to the client.
the process should terminate gracefully.
'''

import socket
import time
import signal
import threading

HOST = '127.0.0.1'
PORT = 7001

# create an event that will be set when we want to stop (Ctrl+C / SIGTERM)
stop_event = threading.Event()

def _handle_signal(signum, frame):
    print("[server] signal received ({}) - shutting down...".format(signum))
    stop_event.set()

# register handlers (SIGTERM may not be available on some platforms)
signal.signal(signal.SIGINT, _handle_signal)
try:
    signal.signal(signal.SIGTERM, _handle_signal)
except AttributeError:
    # Windows may not have SIGTERM; that's fine.
    pass

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((HOST, PORT))
print("UDP server listening on {}:{}".format(HOST, PORT))

start = time.time()
terminate_after = 20  # seconds

try:
    while not stop_event.is_set():
        sock.settimeout(1.0)
        try:
            data, addr = sock.recvfrom(1024)
        except socket.timeout:
            continue
        except OSError:
            # socket may have been closed externally
            break
        if not data:
            continue
        now = time.time()
        elapsed = now - start
        print("[server] recv from {}: {}".format(addr, data.decode(errors='ignore')))
        if elapsed < terminate_after:
            reply = "pong"
        else:
            reply = "terminate"
            print("[server] sending terminate. Exiting after this.")
        try:
            sock.sendto(reply.encode(), addr)
        except OSError:
            break
        if reply == "terminate":
            break
except KeyboardInterrupt:
    print("[server] interrupted by user, shutting down...")
finally:
    try:
        sock.close()
    except Exception:
        pass
    print("server closed")
