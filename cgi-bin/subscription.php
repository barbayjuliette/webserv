#!/usr/bin/php-cgi
<?php
	header("Content-type:text/html\r\n\r\n");

	echo ("<html>");
	echo ('<head>');
	echo ("<title>Newsletter</title>");
	echo ('</head>');
	echo ('<body>');

	$name = '';
	$email = '';

	if ($_SERVER["REQUEST_METHOD"] == "POST")
	{
		if (isset($_POST['name']))
			$name = htmlspecialchars($_POST['name']);
		if (isset($_POST['email']))
			$email = htmlspecialchars($_POST['email']);
	}
	elseif ($_SERVER["REQUEST_METHOD"] == "GET")
	{
		if (isset($_GET['name']))
			$name = htmlspecialchars($_GET['name']);
		if (isset($_GET['email']))
			$email = htmlspecialchars($_GET['email']);
	}

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