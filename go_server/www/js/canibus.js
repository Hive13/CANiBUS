var canibus = angular.module('canibus', []);

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
    .when('/candevice/:id/config',
      {
	controller: 'configController',
	templateUrl: 'partials/config.html'
      })
    .when('/candevice/:id/join',
      {
	controller: 'haxController',
	templateUrl: 'partials/config.html'
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

controllers.lobbyController = function($scope, $http, $timeout) {

  $scope.devices = []

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
    $timeout(function() { $scope.fetchDevices(); }, 3000);
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
  $timeout(function() { $scope.fetchDevices(); }, 3000);
};

controllers.configController = function($scope) {

};

controllers.haxController = function($scope) {

};

canibus.controller(controllers);
