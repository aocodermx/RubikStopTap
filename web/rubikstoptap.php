<?php

  header('Content-Type: application/json; charset=utf-8');

  if ( isset ( $_GET['cube'] ) && isset ( $_GET['single'] ) && isset ( $_GET['average'] ) ) {
    $response = array();
    $cube     = $_GET['cube'];
    $single   = $_GET['single'];
    $average  = $_GET['average'];
  } else {
    http_response_code ( 400 );
    exit ( );
  }

  if ( isset ( $_GET['country'] ) ) {
    $country = $_GET['country'];
  }

  $mysqli = new mysqli ( "localhost", "site_default", "T1M31Sup2015", "site_default" );
  if ( $mysqli->connect_errno ) {
    echo "SQL connect error";
  }


  $result_query = $mysqli->query ( " select worldRank " .
      " from RanksSingle " .
      " where eventId='" . $cube ."' and best > " . $single .
      " order by worldRank limit 1; " );
  $response['cube-world-single'] = intval ( $result_query->fetch_assoc()['worldRank'] );

  $result_query = $mysqli->query ( " select worldRank " .
      " from RanksAverage " .
      " where eventId='" . $cube ."' and best > " . $average .
      " order by worldRank limit 1; " );
  $response['cube-world-average'] = intval ( $result_query->fetch_assoc()['worldRank'] );


  if ( isset ( $country ) ) {
    $result_query = $mysqli->query ( " select continentId from Countries where id = '" . $country . "'; " );
    $continent = $result_query->fetch_assoc()['continentId'];

    $result_query = $mysqli->query ( " select continentRank " .
        " from RanksSingle " .
        " inner join Persons ".
        " on Persons.id = RanksSingle.PersonId " .
        " where Persons.countryId in ( " .
        "   select Id from Countries where Countries.continentId = '" . $continent ."' " .
        " ) and RanksSingle.eventId = '" . $cube ."' and RanksSingle.best > " . $single .
        " order by RanksSingle.continentRank limit 1; " );
    $response['cube-continent-single'] = intval ( $result_query->fetch_assoc()['continentRank'] );

    $result_query = $mysqli->query ( " select continentRank " .
        " from RanksAverage " .
        " inner join Persons" .
        " on Persons.id = RanksAverage.PersonId " .
        " where Persons.countryId in ( " .
        "   select Id from Countries where Countries.continentId = '" . $continent ."'" .
        " ) and RanksAverage.eventId = '" . $cube ."' and RanksAverage.best > " . $single .
        " order by RanksAverage.continentRank limit 1; " );
    $response['cube-continent-average'] = intval ( $result_query->fetch_assoc()['continentRank'] );


    $result_query = $mysqli->query ( " select countryRank " .
        " from RanksSingle " .
        " inner join Persons " .
        " on Persons.id = RanksSingle.PersonId " .
        " where eventId = '" . $cube . "' and best > " . $single . " and countryId = '" . $country . "' " .
        " order by countryRank limit 1; " );
    $response['cube-country-single'] = intval ( $result_query->fetch_assoc()['countryRank'] );

    $result_query = $mysqli->query ( " select countryRank " .
        " from RanksAverage " .
        " inner join Persons " .
        " on Persons.id = RanksAverage.PersonId " .
        " where eventId = '" . $cube . "' and best > " . $average . " and countryId = '" . $country . "' " .
        " order by countryRank limit 1; " );
    $response['cube-country-average'] = intval ( $result_query->fetch_assoc()['countryRank'] );
  }

  /*
  if (!$result_query) {
      throw new Exception("Database Error [{$mysqli->errno}] {$mysqli->error}");
  }
  */

  echo json_encode ( $response );

?>
