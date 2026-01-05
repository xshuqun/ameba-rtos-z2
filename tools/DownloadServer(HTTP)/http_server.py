#openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem

from http.server import HTTPServer, SimpleHTTPRequestHandler
import ssl
import sys

server_ip = '192.168.9.104'
server_port = 443
server_address = (server_ip, server_port)
is_https = 0

httpd = HTTPServer(server_address, SimpleHTTPRequestHandler)
if is_https == 1:
    if sys.hexversion < 0x30a0000:
        print('Python 3.x hexversion %s is in use.' % hex(sys.hexversion))
        ## Below code for python3.9 and below 
        httpd.socket = ssl.wrap_socket(httpd.socket, server_side=True, certfile="./cert.pem", keyfile="./key.pem", ssl_version=ssl.PROTOCOL_TLSv1_2)
    else:
        ## Below code for python3.10 and above
        print('Python 3.x hexversion %s is in use.' % hex(sys.hexversion))
        ctx = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
        ctx.load_cert_chain(certfile="./cert.pem", keyfile="./key.pem")
        httpd.socket = ctx.wrap_socket(httpd.socket, server_side=True)

httpd.serve_forever()
