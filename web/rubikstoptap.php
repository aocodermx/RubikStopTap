<?php


// TODO: get all data in json format.

  $action = $_GET['action'];
  $cube   = $_GET['cube'];
  $time   = $_GET['time'];

  $mysqli = new mysqli ( "localhost", "site_default", "T1M31Sup2015", "site_default" );
  if ( $mysqli->connect_errno ) {
    echo "SQL connect error";
  }

  //echo $mysqli->host_info . "\n";

  switch ( $action ) {
    case "worldsingle":
      $mysqli->real_query( "select worldRank from RanksSingle where eventId='" .
        $cube ."' and best=" . $time ." limit 1" );
      break;
    case "worldaverage":
      $mysqli->real_query( "select worldRank from RanksAverage where eventId='" .
        $cube ."' and best=" . $time ." limit 1;" );
      break;
    case "continentalsingle":
      break;
    case "continentalaverage":
      break;
    case "countrysingle":
      break;
    case "countryaverage":
      break;
    default:
      echo "Bad request.";
      break;
  }

  $result = $mysqli->use_result();

  while ( $fila = $result->fetch_assoc() ) {
    echo $fila['worldRank'];
  }
?>
