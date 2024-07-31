#! /usr/bin/python3
import cgi, cgitb, os

form = cgi.FieldStorage()

blue = form.getvalue('blue')
red = form.getvalue('red')

print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ('<head>')
print ("<title>Hello - CGI Program</title>")
print ('</head>')
print ('<body>')
print (red)
print ("<h2>Blue is  %s</h2>" % (blue))
print ("<h2>Red is  %s</h2>" % (red))


# Debugging: Print all environment variables
print("<h3>Environment Variables</h3>")
# print("<pre>")
for key in os.environ.keys():
    print("%s = %s" % (key, os.environ[key]))
# print("</pre>")

# Debugging: Print form fields
print("<h3>Form Fields</h3>")
# print("<pre>")
for key in form.keys():
    print("%s = %s" % (key, form.getvalue(key)))
# print("</pre>")

print("</body>")
print("</html>")

# print ('</body>')
# print ('</html>')