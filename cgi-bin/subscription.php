<html>
<body>

<?php
   print_r($_POST);
   print_r($_REQUEST);
?>

Welcome <?php echo $_POST["name"]; ?><br>
Your email address is: <?php echo $_POST["email"]; ?>

</body>
</html>