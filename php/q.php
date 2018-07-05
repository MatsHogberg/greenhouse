<?php
    require "secrets.php";

    $myObj = new \stdClass();
    $sunUpDown = file_get_contents("https://api.sunrise-sunset.org/json?lat=58.132132&lng=11.603812&formatted=1");
    $sunUpDown = json_decode($sunUpDown);
    $sunDownString = $sunUpDown->results->sunset;
    $nu = time();
    $afterSunset = $nu > strtotime($sunDownString);
    if($afterSunset){
        $conn = openDb($dbConnection, $dbUser, $dbPassword, $database);
        $r = getTempAndHumidity($conn);
        $humidity = $r->humidity;
        if($humidity < 500){
            echo 1;
        }else{
            echo 0;
        }
    }else{
        echo 0;
    }

    function getTempAndHumidity($conn){
        $ret = new \stdClass();
        $sql = "SELECT value FROM greenhouse WHERE sensor = 'sh' ORDER BY timestamp DESC LIMIT 1";
        if($result = $conn->query($sql)){
            if($result->num_rows ===1){
                while($row = $result->fetch_assoc()){
                    $h = $row["value"];
                }
            }
        }
        $ret->humidity = $h;
        }
    function openDb($dbConnection, $dbUser, $dbPassword, $database){
        $mysqli = new mysqli("$dbConnection", "$dbUser", "$dbPassword", "$database");
        if($mysqli->connect_errno){
            echo $mysqli->connect_errno;
            exit();
        }
        return $mysqli;
    }

?>