<?php
    require "secrets.php";

    $myObj = new \stdClass();
    $sunUpDown = file_get_contents("https://api.sunrise-sunset.org/json?lat=58.132132&lng=11.603812&formatted=1");
    $sunUpDown = json_decode($sunUpDown);
    $sunDownString = $sunUpDown->results->sunset;
    $nu = time();
    echo strtotime($sunDownString) - $nu;
?>