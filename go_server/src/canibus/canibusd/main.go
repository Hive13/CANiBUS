// Main canibus server
package main

import (
	"canibus/core"
	"canibus/server"
	"canibus/webserver"
	"flag"
	"os"
)

const (
	DEFAULT_IP   = "0.0.0.0"
	DEFAULT_PORT = "1234"
        DEFAULT_WEBPORT = "2515"
	DEFAULT_WWW_ROOT = "www"
	DEFAULT_CONFIG_FILE = "config.json"
)

var ServerConfig server.Config

var bindIP = flag.String("ip", DEFAULT_IP, "IP to bind to")
var tcpPort = flag.String("port", DEFAULT_PORT, "TCP port")
var wwwPort = flag.String("www", DEFAULT_WEBPORT, "port for web server")
var wwwRoot = flag.String("root", DEFAULT_WWW_ROOT, "file path for web server")
var configFile = flag.String("config", DEFAULT_CONFIG_FILE, "Settings config file")

func launchTCPServer() {
	err := server.StartListener(*bindIP, *tcpPort)
	if err != nil {
		println(err.Error())
		os.Exit(1)
	}
}

func launchWebServer() {
	err := webserver.StartWebListener(*wwwRoot, *bindIP, *wwwPort)
	if err != nil {
		println(err.Error())
		os.Exit(1)
	}

}

func main() {
	flag.Parse()
	core.SetConfig(&ServerConfig)
	core.LoadConfig(*configFile)
	server.InitDrivers()
	go launchTCPServer()
	launchWebServer()
}
