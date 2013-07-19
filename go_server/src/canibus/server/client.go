package server

import (
	"canibus/api"
	"canibus/logger"
	"encoding/json"
	"encoding/xml"
	"fmt"
	"net"
)

const (
	LANG_XML = iota
	LANG_JSON
)

const (
	STATE_UNAUTH = iota
	STATE_LOBBY
)

// Main Client Structure
type Client struct {
	Name     string
	Incoming chan string
	Outgoing chan string
	Conn     net.Conn
	Quit     chan bool
	Lang     int
	State	int
	Id	int
}

// Closes the clients connection
func (c *Client) Close() {
	c.Quit <- true
	c.Conn.Close()
}

// ClientReader reads data from the client socket
func ClientReader(client *Client) {
	buffer := make([]byte, 2048)
	for {
		n, err := client.Conn.Read(buffer)
		if err != nil {
			fmt.Printf("Error: reading data: %s\n", err)
			client.Close()
			break
		}
		client.Incoming <- string(buffer[0:n])
	}
}

// ClientWriter writes data to the client socket
func ClientWriter(client *Client) {
	for {
		select {
		case buffer := <-client.Outgoing:
			client.Conn.Write([]byte(buffer))
		case <-client.Quit:
			client.Close()
		}
	}
}

// ProcessOutgoing formats API to client request output syntax
func (c *Client) ProcessOutgoing(apiStruct interface{}) {
	var outgoing string
	switch c.Lang {
	case LANG_JSON:
		x, err := json.Marshal(apiStruct)
		if err != nil {
			logger.Log("Could not create JSON server message")
			return
		}
		outgoing = string(x)
	default:
		x, err := xml.Marshal(apiStruct)
		if err != nil {
			logger.Log("Could not create XML server message")
			return
		}
		outgoing = string(x)
	}
	if outgoing != "" {
		c.Outgoing <- outgoing + "\n"
	}
}

// ProcessIncoming unmarshals string request back to structs
func (c *Client) ProcessIncoming(msg string) (*api.Cmd, error) {
	cmd := &api.Cmd{}
	switch c.Lang {
	case LANG_JSON:
		err := json.Unmarshal([]byte(msg), &cmd)
		if err != nil {
			return cmd, logger.Err("Invalid JSON Syntax")
		}
	default: // LANG_XML
		err := xml.Unmarshal([]byte(msg), &cmd)
		if err != nil {
			return cmd, logger.Err("Invalid XML Syntax")
		}
	}
	return cmd, nil
}

func (c *Client) sendError(t string, msg string) {
	err := &api.Err{ t, msg }
	c.ProcessOutgoing(err)
}

// ProcessLogin assumes Arg0 = Name
func (c *Client) ProcessLogin(cmd *api.Cmd) {
	switch len(cmd.Arg) {
	case 1:
		if cmd.Arg[0] == "" {
			c.sendError("Login","Invalid Name")
			return
		}
		c.Name = cmd.Arg[0] // Do we want to make this uniq?
		c.State = STATE_LOBBY
		logger.Log(c.Name + " logged in")
	default:
		c.sendError("Login", "Wrong number of arguments needed to login")
	}
}

// ProcessCommand processes client commands from the network socket
func (c *Client) ProcessCommand(cmd string) {
	//fmt.Println("DEBUG: process cmd: ", cmd)
	aCmd, err := c.ProcessIncoming(cmd)
	if err != nil {
		fmt.Println(err)
		return
	}
	switch aCmd.Action {
	case "Login":
		c.ProcessLogin(aCmd)
	case "":
		logger.Log("Invalid Cmd request")
		return
	default:
		logger.Log("Unkown action:" + aCmd.Action)
	}
}


