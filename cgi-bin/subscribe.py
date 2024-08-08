#! /usr/bin/python3
import cgi, cgitb, os
from datetime import datetime

form = cgi.FieldStorage()

print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ('<head>')
print ("<title>Newsletter</title>")
print ('</head>')
print ('<body>')

email = form.getvalue('email')

if email is None or email is "":
	print ('<p>No data received from form, please try again</p>')
else:
	now = datetime.today().strftime('%Y-%m-%d %H:%M:%S')
	print ("<h2>Thank you %s</h2>" % (email))
	print ( "<p>for subscribing to our newsletter on %s</p>" % (now))

print ('</body>')
print ('</html>')