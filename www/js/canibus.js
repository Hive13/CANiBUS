var canibus = angular.module('canibus', ['ngRoute', 'ngAnimate']);

var controllers = {};

canibus.config(function ($routeProvider) {
  $routeProvider
    .when("/",
      {
	controller: 'loginController',
	templateUrl: 'partials/login.html'
      })
    .when('/lobby',
      {
	controller: 'lobbyController',
	templateUrl: 'partials/lobby.html'
      })
    .when('/candevice/:id/:action',
      {
	controller: 'configController',
	templateUrl: '/partials/urlRouter.html'
      })
    .when('/hax/:id',
      {
	controller: 'haxController',
	templateUrl: '/partials/sniff.html'
      })
    .otherwise({ redirectTo: "/" });
});

controllers.loginController = function($scope, $http, $location) {

  $scope.login = function() {
    var loginData = "username=" + $scope.username;
    $http({
      url: "/login",
      method: "POST",
      data: loginData,
      headers: {'Content-Type': 'application/x-www-form-urlencoded'}
    }).success(function (data, status) {
	 if (status == 200) {
	  $location.path('/lobby');
	 } else {
	  $location.path('/');
         }
       }).error(function (data, status) {
          $location.path('/');
       });
  }

};

controllers.lobbyController = function($scope, $location, $http, $timeout) {

  $scope.devices = []
  var devicePromise;

  $scope.$on("$destroy", function() {
    if(devicePromise) {
      $timeout.cancel(devicePromise);
    }
  });

  $scope.config = function(id) {
    if(devicePromise) {
      $timeout.cancel(devicePromise);
    }
  }

  $scope.join = function(id) {
    if(devicePromise) {
      $timeout.cancel(devicePromise);
    }
  }

  function DevInList(dev) {
    for (var i = 0; i < $scope.devices.length; i++) {
      if($scope.devices[i].Id == dev.Id) {
        return true;
      }
    }
    return false;
  }

  function addDevices(devices) {
    var changed = false;
    angular.forEach(devices, function(dev) {
      if (!DevInList(dev)) {
        $scope.devices.push(dev);
        changed = true;
      } else {
        for(var i = 0; i < $scope.devices.length; i++) {
          if($scope.devices[i].Id == dev.Id) {
            if($scope.devices[i].HackSession != dev.HackSession) {
              $scope.devices[i].HackSession = dev.HackSession;
              changed = true;
            }
          }
        }
      }
    });
    devicePromise = $timeout(function() { $scope.fetchDevices(); }, 3000);
  }

  $scope.fetchDevices = function() {
    $http.get("/candevices").success(function(data, status) {
      addDevices(data);
    });
  }

  $scope.AddSimulator = function() {
    $http.get("/lobby/AddSimulator").success(function(data, status) {
      $scope.devices.push(data);
    });
  }

  $scope.fetchDevices();
};

controllers.configController = function($scope, $http, $location, $routeParams) {
  $scope.isMaster = function() {
    if($location.path().indexOf("config", $location.path().length - 6) == 13) {
      return true;
    }
    return false;
  }

  $scope.TemplateUrl = 'candevice/' + $routeParams.id + '/' + $routeParams.action;

  $scope.getInfo = function(id) {
    $http.get("/candevice/" + id + "/info").success(function(data, status) {
      $scope.device = data
    });
  }

  $scope.getInfo($routeParams.id);  

};

controllers.haxController = function($scope, $filter, $http, $timeout, $routeParams) {
  $scope.packets = [];
  $scope.id = $routeParams.id;
  $scope.predicate = "ArbID";
  $scope.reverse = false;
  $scope.transmitCount = 0;
  $scope.transmitErr = "";
  $scope.tx = {ArbId: '', Network: '', B1: '', B2: '', B3: '', B4: '', B5: '', B6: '', B7: '', B8: ''};

  $scope.viewType = "ArbView";
  var snifferPromise;
  var pollTimer = 500;

  function PacketArbIDInList(pkt) {
    for (var i = 0; i < $scope.packets.length; i++) {
      if($scope.packets[i].ArbID == pkt.ArbID && $scope.packets[i].Network == pkt.Network) {
        return true;
      }
    }
    return false;
  }

  function PacketInList(pkt) {
    for (var i = 0; i < $scope.packets.length; i++) {
      if($scope.packets[i].SeqNo == pkt.SeqNo) {
        return true;
      }
    }
    return false;
  }

  function addPackets(packets) {
    var changed = false;
    if(packets == "null") {
      // Blank return...try again in a bit
      snifferPromise = $timeout(function() { $scope.fetchPackets($scope.id); }, pollTimer);
      return;
    }
    angular.forEach(packets, function(pkt) {
      if ($scope.viewType == 'SeqView') {
        if (!PacketInList(pkt)) {
          $scope.packets.push(pkt);
          changed = true;
        }
      } else if ($scope.viewType == 'ArbView') {
        if (!PacketArbIDInList(pkt)) {
          $scope.packets.push(pkt);
          changed = true;
        } else {
          for(var i = 0; i < $scope.packets.length; i++) {
            if($scope.packets[i].ArbID == pkt.ArbID && $scope.packets[i].Network == pkt.Network) {
              if($scope.packets[i].B1 != pkt.B1) {
                $scope.packets[i].B1 = pkt.B1;
                $scope.packets[i].B1changed = true;
                changed = true;
              } else {
                $scope.packets[i].B1changed = false;
              }
              if($scope.packets[i].B2 != pkt.B2) {
                $scope.packets[i].B2 = pkt.B2;
                $scope.packets[i].B2changed = true;
                changed = true;
              } else {
                $scope.packets[i].B2changed = false;
              }
              if($scope.packets[i].B3 != pkt.B3) {
                $scope.packets[i].B3 = pkt.B3;
                $scope.packets[i].B3changed = true;
                changed = true;
              } else {
                $scope.packets[i].B3changed = false;
              }
              if($scope.packets[i].B4 != pkt.B4) {
                $scope.packets[i].B4 = pkt.B4;
                $scope.packets[i].B4changed = true;
                changed = true;
              } else {
                $scope.packets[i].B4changed = false;
              }
              if($scope.packets[i].B5 != pkt.B5) {
                $scope.packets[i].B5 = pkt.B5;
                $scope.packets[i].B5changed = true;
                changed = true;
              } else {
                $scope.packets[i].B5changed = false;
              }
              if($scope.packets[i].B6 != pkt.B6) {
                $scope.packets[i].B6 = pkt.B6;
                $scope.packets[i].B6changed = true;
                changed = true;
              } else {
                $scope.packets[i].B6changed = false;
              }
              if($scope.packets[i].B7 != pkt.B7) {
                $scope.packets[i].B7 = pkt.B7;
                $scope.packets[i].B7changed = true;
                changed = true;
              } else {
                $scope.packets[i].B7changed = false;
              }
              if($scope.packets[i].B8 != pkt.B8) {
                $scope.packets[i].B8 = pkt.B8;
                $scope.packets[i].B8changed = true;
                changed = true;
              } else {
                $scope.packets[i].B8changed = false;
              }
            }
          }
        }
      }
    });
    snifferPromise = $timeout(function() { $scope.fetchPackets($scope.id); }, pollTimer);
  }

  $scope.StopSniffer = function(id) {
    $http.get("/hax/" + id + "/stop").success(function(data, status) {
      $scope.started = false;
      $timeout.cancel(snifferPromise);
    });
  }

  $scope.StartSniffer = function(id) {
    $http.get("/hax/" + id + "/start").success(function(data, status) {
      $scope.started = true;
      snifferPromise = $timeout(function() { $scope.fetchPackets($scope.id); }, pollTimer);
    });
  }

  $scope.setArbView = function() {
    $scope.viewType='ArbView';
    $scope.packets = [];
  }

  $scope.setSeqView = function() {
    $scope.viewType='SeqView';
    $scope.packets = [];
  }

  $scope.transmit = function(id) {
    if($scope.tx == "" || $scope.tx.ArbId == "") {
      $scope.transmitErr = "You must specify an ArbId";
      return;
    }
    pkt = "[" + JSON.stringify($scope.tx) + "]";
    $http({
      url: "/hax/"+id+"/transmit",
      method: "POST",
      data: "tx=" + pkt,
      headers: {'Content-Type': 'application/x-www-form-urlencoded'}
    }).success(function (data, status) {
         $scope.transmitCount += 1;
         $scope.transmitErr == "";
       }).error(function (data, status) {
         $scope.transmitErr == "Could not transmit packet";
       });
  }

  $scope.copyPacket = function(pkt) {
    this.tx.ArbId = pkt.ArbID;
    $scope.tx.Network = pkt.Network;
    $scope.tx.B1 = pkt.B1;
    $scope.tx.B2 = pkt.B2;
    $scope.tx.B3 = pkt.B3;
    $scope.tx.B4 = pkt.B4;
    $scope.tx.B5 = pkt.B5;
    $scope.tx.B6 = pkt.B6;
    $scope.tx.B7 = pkt.B7;
    $scope.tx.B8 = pkt.B8;
  }

  $scope.fetchPackets = function(id) {
    $http.get("/hax/" + id + "/packets").success(function(data, status) {
      addPackets(data);  
    });
  }
};

canibus.controller(controllers);
