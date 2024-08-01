#! /usr/bin/python3
import cgi, cgitb, os
from datetime import datetime

form = cgi.FieldStorage()

email = form.getvalue('email')
now = datetime.today().strftime('%Y-%m-%d %H:%M:%S')

print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ('<head>')
print ("<title>Newsletter</title>")
print ('</head>')
print ('<body>')
print ("<h2>Thank you %s</h2>" % (email))
print ( "<p>for subscribing to our newsletter on %s</p>" % (now))
print ('</body>')
print ('</html>')