Pebble.addEventListener('ready', function(e) {
  console.log('JavaScript app running and ready!');
});

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('Received message: ' + JSON.stringify(e.payload));
    getWorldStats ( e.payload["cube-size"], e.payload["cube-min"], e.payload["cube-avg"] );
  }
);

function getWorldStats ( cubesize, cubemin, cubeavg ) {
  var req = new XMLHttpRequest();
  var url = 'http://raspberrypi.lan/rubiksite/rubikstoptap.php?' +
  'action=' + 'worldaverage' + '&' +
  'cube=' + cubesize + cubesize + cubesize + '&' +
  'time=' + cubeavg;

  console.log ( 'Request url: ' + url );

  req.open('GET', url, true);

  req.onload = function ( e ) {
    if (req.readyState == 4) {
      if(req.status == 200) {

        console.log(req.responseText);
        Pebble.sendAppMessage ( {
          'cube-world-avg': parseInt ( req.responseText ),
          'cube-world-min': 0
        }, function (e) {
          console.log ( 'Ranking info sent successfully.' );
        }, function (e) {
          console.log ( 'Ranking info sent failed.' );
        });

      } else {

        console.log("Request returned error code " + req.status.toString());

      }
    }
  }

  req.send(null);
}
