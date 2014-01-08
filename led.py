#!/usr/bin/python
from BaseHTTPServer import HTTPServer;
from SimpleHTTPServer import SimpleHTTPRequestHandler;
import sys, os, subprocess;


class HTTPHandler (SimpleHTTPRequestHandler):
	server_version = "MonServeurHTTP/0.1";
	def do_GET(self):
		self.send_response(200, 'OK');
		self.send_header('Content-type', 'image/jpeg');
		self.end_headers();
		with open('./led.jpeg', 'r') as fp:
			self.wfile.write(fp.read());

print 'Taking screenshot';
#subprocess.Popen(['streamer', '-f', 'jpeg', '-j', '100', '-s', '1024x768', '-o', './led.jpeg'], stdout=subprocess.PIPE).communicate();
# http://manpages.ubuntu.com/manpages/lucid/man1/fswebcam.1.html
res = subprocess.Popen(['fswebcam', '--jpeg', '100', '--save', 'led.jpeg', '--frame', '10', '--resolution', '1024x768', '--set', 'brightness=50%'], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate();
print 'Done: http://109.190.12.108:660/';
httpd = HTTPServer(('', 8080), HTTPHandler);
httpd.handle_request();
os.remove('./led.jpeg');
