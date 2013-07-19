// Main canibus server
package main

import (
	"canibus/server"
	"canibus/webserver"
	"os"
)

const (
	DEFAULT_IP   = "0.0.0.0"
	DEFAULT_PORT = "1234"
        DEFAULT_WEBPORT = "2515"
	DEFAULT_WWW_ROOT = "www"
)

func launchTCPServer() {
	err := server.StartListener(DEFAULT_IP, DEFAULT_PORT)
	if err != nil {
		println(err.Error())
		os.Exit(1)
	}
}

func launchWebServer() {
	err := webserver.StartWebListener(DEFAULT_WWW_ROOT, DEFAULT_IP, DEFAULT_WEBPORT)
	if err != nil {
		println(err.Error())
		os.Exit(1)
	}

}

func main() {
	go launchTCPServer()
	launchWebServer()
}
