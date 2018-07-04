<?php
    require "secrets.php";

    $myObj = new \stdClass();

    if(isset($_POST['data'])){
        $s=$_POST['data'];

        // $s = '{"delay":15,"hum_in":33,"temp_in":14.4,"hum_out":81,"temp_out":-0.2,"abs_pressure":999,"wind_ave":0,"wind_gust":0,"wind_dir":270,"rain":29.4,"status":{"b1":false,"b2":false,"b3":false,"b4":false,"b5":false,"lost_sensor_contact":false,"rain_overflow":false,"b8":false},"unix":1521485640000,"datetime":"2018-03-19T18:54:00.000Z","lastUpdated":"2018-03-19T19:09:00.000Z"}';
        
        $obj = json_decode($s);

        $airTemp = $obj->{'t'};
        $airHumidity = $obj->{'ah'};
        $soilHumidity = $obj->{'sh'};

        $conn = openDb($dbConnection, $dbUser, $dbPassword, $database);
        $r = insert($conn, $airTemp, "t");
        $r = insert($conn, $airHumidity, "ah");
        $r = insert($conn, $soilHumidity, "sh");
        
        $conn->close();
        $myObj->result ="OK";
        $myObj->dbResult = $r;
        $myObj->t =$airTemp;
        $myObj->ah =$airHumidity;
        $myObj->sh =$soilHumidity;
        

    }else{
        $myObj->result="Error in key";
    }

    echo json_encode($myObj);

    function insert($conn, $t, $l){
        $sql ="INSERT INTO greenhouse(value, sensor) VALUES($t, '$l');";
        if($result = $conn->query($sql)){
            return $result;
        }else{
            return "ERROR: " . $sql;
        }
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