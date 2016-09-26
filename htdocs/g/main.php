<?php
	$db = new mysqli("localhost", "root", "", "simplebot"); //connect to db with root and empty password credentials
	if (mysqli_connect_errno()){die('Query error: '.$db->error);} //error connecting to mysql?

	$result = $db->query("SELECT * FROM `bots` ORDER BY `Timestamp` DESC LIMIT 0,30"); //select first 30 bots, sort by last seen on top. next 30 would be LIMIT 30,60
	if(!$result){die ('Query error: '.$db->error);}
	
	echo('<body bgcolor="#000000"><table border="1" width="100%"><tr style="color:lightgrey; font-size:30;"><th align="center">ID</th><th align="center">IP</th><th align="center">PC-Name</th><th align="center">Username</th><th align="center">Last seen</th></tr>');
	while ($row = $result->fetch_assoc()) //loop throug result and print them
	{
		echo('<tr style="color:lightgrey; font-size:20;">');
		echo('<td align="center">' . $row["ID"] . '</td>');
		echo('<td align="center">' . long2ip($row["IP"]) . '</td>');
		echo('<td align="center">' . $row["PC-Name"] . '</td>');
		echo('<td align="center">' . $row["Username"] . '</td>');
		echo('<td align="center">' . $row["Timestamp"] . '</td></tr>');
	}
	echo('</table>');
	
	$result->close(); //cleanup
	unset($result);
	unset($db);
	return;
?>