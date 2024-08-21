#!/usr/bin/php-cgi
<?php
	header("Content-type:text/html\r\n\r\n");

	echo ("<html>");
	echo ('<head>');
	echo ("<title>Newsletter</title>");
	echo ('</head>');
	echo ('<body>');

	$name = isset($_POST['name']) ? htmlspecialchars($_POST['name']) : '';
	$email = isset($_POST['email']) ? htmlspecialchars($_POST['email']) : '';

	if (empty($name) || empty($email))
	{
		echo ("Name and email are required!");
		exit;
	}

	echo ("<h2>Welcome $name!</h2>");
	echo ("<p>You have successfully subscribed with this email: $email.</p>");

	echo ('</body>');
	echo ('</html>');
?>