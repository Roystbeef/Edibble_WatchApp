var id;

Pebble.addEventListener('ready', function() {
  console.log("Pebble JS is ready!");
});

Pebble.addEventListener('appmessage', function(e) {
  console.log("message");
  console.log(e.payload.request);
  switch(e.payload.request) {
    case 0:
      getSeated();
      break;
    case 1:
      reqOrder();
      break;
    case 2:
      reqRefill();
      break;
    case 3:
      reqCheck();
      break;
    case 4:
      reqHelp();
      break;
  }
});

var getSeated = function() {
  var request = new XMLHttpRequest();
  console.log("requested");
  request.open('GET',
               'http://192.168.42.1:3000/start',
              true);
  request.onload = sendSeat;
  request.send();
  console.log(request);
};

var sendSeat = function(xhr) {
  var response = JSON.parse(xhr.target.response);
  console.log(response);
  id = response.customerId;
  console.log(id);
  Pebble.sendAppMessage({"id": id});
};

var reqOrder = function() {
  var request = new XMLHttpRequest();
  console.log("requested");
  request.open('GET',
               'http://192.168.42.1:3000/readyToOrder/' + id,
              true);
  request.onload = req;
  request.send();
  console.log(request);
};

var reqRefill = function() {
  var request = new XMLHttpRequest();
  console.log("requested");
  request.open('GET',
               'http://192.168.42.1:3000/requestRefill/:' + id,
              true);
  request.onload = req;
  request.send();
  console.log(request);
};

var reqCheck = function() {
  var request = new XMLHttpRequest();
  console.log("requested");
  request.open('GET',
               'http://192.168.42.1:3000/requestCheck/' + id,
              true);
  request.onload = req;
  request.send();
  console.log(request);
};

var reqHelp = function() {
  var request = new XMLHttpRequest();
  console.log("requested");
  request.open('GET',
               'http://192.168.42.1:3000/requestAssistance/' + id,
              true);
  request.onload = req;
  request.send();
  console.log(request);
};

var req = function(xhr) {
  
};