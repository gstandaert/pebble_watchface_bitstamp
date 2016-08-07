var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

// {"high": "329.39", "last": "321.68", "timestamp": "1448036721", "bid": "321.80", "vwap": "319.97", "volume": "21404.75974242", "low": "310.00", "ask": "321.87", "open": 326.51}
function getBitstampData() {
  var url = 'https://www.bitstamp.net/api/ticker/';
  xhrRequest(url, 'GET', 
    function(responseText) {
      // responseText contains a JSON object with the bitstamp info
      var json = JSON.parse(responseText);

      var bitstampLastPrice =json.last;
      var bitstampLowPrice =json.low;
      var bitstampHighPrice =json.high;
      console.log('Bitstamp Last Price is ' + bitstampLastPrice);
      console.log('Bitstamp Low Price is ' + bitstampLowPrice);
      console.log('Bitstamp High Price is ' + bitstampHighPrice);
        
      // Assemble dictionary using our keys
      var dictionary = {
        'KEY_BITSTAMP_LASTPRICE': bitstampLastPrice.toString(),
        'KEY_BITSTAMP_LOWPRICE': bitstampLowPrice.toString(),
        'KEY_BITSTAMP_HIGHPRICE': bitstampHighPrice.toString()
      };
     
      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('Bitstamp info sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending Bitstamp info to Pebble!');
        }
      );
    }      
  );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    getBitstampData();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    getBitstampData();
  }                     
);