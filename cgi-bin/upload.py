#! /usr/bin/python3
import cgi, os
import cgitb; cgitb.enable()

form = cgi.FieldStorage()

fileitem = form['file']
print("Form keys: ", form.keys())

if fileitem.filename:
	fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
	if os.path.isdir("./uploads") is False :
		os.makedirs("./uploads")
	file = open('./uploads/' + fn, 'wb')
	file.write(fileitem.file.read())
	message = 'The file "' + fn + '" was uploaded successfully'
   
else:
	message = 'No file was uploaded'

print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ('<head>')
print ("<title>File upload</title>")
print ('</head>')
print ('<body>')
print ("<h2>%s</h2>" % (message))
print ('</body>')
print ('</html>')