/*
  Package Server contains the functions to handle network communications.
*/
package server

import (
	"canibus/api"
	"canibus/logger"
	"container/list"
	"fmt"
	"net"
	"strings"
)

type ServerData struct {
	ClientList *list.List
}

// Global Data
var GData ServerData

// Initializes common variables and structures
func serverInit() {
	api.InitAPI()
	GData.ClientList = list.New()
}

// StartListener starts a listening socket on a given port and IP
func StartListener(ip string, port string) error {
	serverInit()
	remote := ip + ":" + port
	logger.Log("Starting CANiBUS Server on " + remote)
	ln, err := net.Listen("tcp", remote)
	if err != nil {
		return logger.Err("Could not bind to port: " + err.Error())
	}
	logger.Log("Server started")
	defer ln.Close()
	for {
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println(err)
			continue
		}
		go handleConnection(conn)
	}
	return nil
}

func handleConnection(conn net.Conn) {
	logger.Log(fmt.Sprintf("Incoming Connection from %s", conn.RemoteAddr()))
	buffer := make([]byte, 1024)
	bytesRead, error := conn.Read(buffer)
	if error != nil {
		logger.Log(fmt.Sprintf("Error reading data: %s", error))
		Close(conn)
		return
	}
	newClient := &Client{}
	nudge := string(buffer[0:bytesRead])
	switch strings.TrimSpace(nudge) {
	case "lang:xml":
		newClient.Lang = LANG_XML
	case "lang:json":
		newClient.Lang = LANG_JSON
	default:
		logger.Log("Invalid nudge string")
		Close(conn)
		return
	}
	// Valid nugde string
	newClient.Incoming = make(chan string)
	newClient.Outgoing = make(chan string)
	newClient.Conn = conn
	newClient.Quit = make(chan bool)

	go ClientWriter(newClient)
	go ClientReader(newClient)
	GData.ClientList.PushBack(*newClient)

	go HandleIncoming()

	newClient.ProcessOutgoing(api.ServerVersion)
}

func Close(conn net.Conn) {
	logger.Log(fmt.Sprintf("%s closed connection", conn.RemoteAddr()))
	conn.Close()
}

// HandleIncoming handles incomming messages from clients
func HandleIncoming() {
	for {
		for c := GData.ClientList.Front(); c != nil; c = c.Next() {
			client := c.Value.(Client)
			msg := <-client.Incoming
			msg = strings.TrimSpace(msg)
			if msg != "" {
				client.ProcessCommand(msg)
			}
		}
	}
}
