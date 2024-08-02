#! /usr/bin/python3
import cgi, cgitb, os
from datetime import datetime

now = datetime.today().strftime('%Y-%m-%d %H:%M:%S')

print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ('<head>')
print ("<title>CGI GET</title>")
print ('</head>')
print ('<body>')
print ("<h2>Current date and time: %s</h2>" % (now))
print ('</body>')
print ('</html>')