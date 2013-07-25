// Main canibus server
package main

import (
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
)

var bindIP = flag.String("ip", DEFAULT_IP, "Default IP to bind to")
var tcpPort = flag.String("port", DEFAULT_PORT, "Default TCP port")
var wwwPort = flag.String("www", DEFAULT_WEBPORT, "Default port for web server")
var wwwRoot = flag.String("root", DEFAULT_WWW_ROOT, "Default file path for web server")

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
	go launchTCPServer()
	launchWebServer()
}
