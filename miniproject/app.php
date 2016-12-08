<?php
$timeout = 5; // seconds

// read app.json
$json = json_decode(file_get_contents("app.json"), true);

// delete old entries
foreach ($json as $id => $device) {
  if($device["time"] < time() - $timeout) {
    unset($json[$id]);
  }
}

if (isset($_POST["id"]) && !empty($_POST["id"])) {
  // update entry for device
  $id = htmlspecialchars($_POST["id"]);
  $json[$id] = array("time" => time());
  echo "updated";
} else {
  // return json of devices
  header('Content-Type: application/json');
  echo json_encode($json);
}

// save app.json
$file = fopen("app.json","w");
fwrite($file, json_encode($json,JSON_PRETTY_PRINT));
fclose($file);
?>
