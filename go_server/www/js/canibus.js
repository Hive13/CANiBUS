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

controllers.lobbyController = function($scope, $http) {

  $http.get("/candevices").success(function(data, status) {
    $scope.devices = data
  });
};

controllers.configController = function($scope) {

};

controllers.haxController = function($scope) {

};

canibus.controller(controllers);
