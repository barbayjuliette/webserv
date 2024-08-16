#!/usr/bin/php
<?php
// Output the Content-Type header
// header("Content-Type: text/html");
echo "Content-type:text/html\r\n\r\n";
// file_put_contents('./test.log', print_r($GLOBALS, true));

echo "<h2>Debugging Information</h2>";
echo "<pre>";
print_r($_POST);
print_r($_REQUEST);
echo "</pre>";

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Subscription Result</title>
</head>
<body>
    <h1>Subscription Result</h1>

    <h2>Submitted Data</h2>

    <?php
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Check if 'name' and 'email' fields are set before accessing them
        if (isset($_POST['name']) && isset($_POST['email'])) {
            $name = htmlspecialchars($_POST['name']);
            $email = htmlspecialchars($_POST['email']);
            echo "<p>POST Method:</p>";
            echo "<p>Name: $name</p>";
            echo "<p>Email: $email</p>";
        } else {
            echo "<p>No data submitted via POST.</p>";
        }
    } elseif ($_SERVER["REQUEST_METHOD"] == "GET") {
        // Check if 'name' and 'email' fields are set before accessing them
        if (isset($_GET['name']) && isset($_GET['email'])) {
            $name = htmlspecialchars($_GET['name']);
            $email = htmlspecialchars($_GET['email']);
            echo "<p>GET Method:</p>";
            echo "<p>Name: $name</p>";
            echo "<p>Email: $email</p>";
        } else {
            echo "<p>No data submitted via GET.</p>";
        }
    }
    ?>
</body>
</html>
