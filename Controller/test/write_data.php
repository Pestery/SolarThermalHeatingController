#!/usr/bin/php-cli
<?php
$var1 = $argv[1];  //timestamp for station 1 and 2
$var2 = $argv[2];  //station 1
$var3 = $argv[3];  //station 2
$var4 = $argv[4];  //adjustmentdial
$var5 = $argv[5];  //station 3
$var6 = $argv[6];  //station 4
$var7 = $argv[7];  //timestamp for station 3 and 4


$DBServer = 'localhost:8306'; 
$DBUser   = 'dbuser';
$DBPass   = 'userpass';
$DBName   = 'Project39DB';  
$conn = new mysqli($DBServer, $DBUser, $DBPass, $DBName);
// check connection
if ($conn->connect_error) {
  trigger_error('Database connection failed: '  . $conn->connect_error, E_USER_ERROR);
}
//$sql="INSERT INTO assy5cp (timestamp, station1, station2, adjustmentdial) VALUES ($var1,$var2,$var3,$var4);
//UPDATE assy5cp SET station3=$var5, station4=$var6 WHERE timestamp=$var7;";
//if($conn->multi_query($sql) === false) {
 // trigger_error('Wrong SQL: ' . $sql . ' Error: ' . $conn->error, E_USER_ERROR);
//} 
?>