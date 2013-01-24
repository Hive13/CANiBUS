// Main canibus server
package main

import (
	"canibus/server"
	"os"
)

const (
	DEFAULT_IP   = "0.0.0.0"
	DEFAULT_PORT = "1234"
)

func main() {
	err := server.StartListener(DEFAULT_IP, DEFAULT_PORT)
	if err != nil {
		println(err.Error())
		os.Exit(1)
	}
}
