#! /usr/bin/python3
import cgi, os
import cgitb; cgitb.enable()

form = cgi.FieldStorage()

fileitem = form['file']
print("Form keys: ", form.keys())

if fileitem.filename:
	# strip leading path from file name to avoid 
	# directory traversal attacks
	print('in IF')
	fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
	file = open('./uploads/' + fn, 'wb')
	print("this is fn: %s", fn)
	print("this is file: %s", file)
	file.write(fileitem.file.read())
	message = 'The file "' + fn + '" was uploaded successfully'
   
else:
	print("in else")
	message = 'No file was uploaded'

print ("Content-type:plain/text\r\n\r\n")
print ("<html>")
print ('<head>')
print ("<title>File upload</title>")
print ('</head>')
print ('<body>')
print ("<h2>%s</h2>" % (message))
print ('</body>')
print ('</html>')