// Package API implements the API used by the CANiBUS Server
package api

import (
	"canibus/logger"
	"fmt"
)

type Configer interface {
	GetDrivers() []CanDevice
	LoadConfig(conf string)
	AppendDriver(CanDevice) int
}

type User interface {
	GetName() string
	SetName(name string)
	GetDeviceId() int
	SetDeviceId(int)
	LastIdx() int
	SetLastIdx(int)
}

type CanDevice interface {
        Init() bool
        DeviceType() string
        DeviceDesc() string
        GetId() int
        SetId(int)
        GetHackSession() HackSession
        SetHackSession(HackSession)
	GetYear() string
	GetMake() string
	GetModel() string
	GetPacketsFrom(int) ([]CanData, int)
	GetPacketIdx() int
	StartSniffing()
	StopSniffing()
}

type HackSession interface {
	GetState() string
	GetStateValue() int
	SetState(int)
	GetDeviceId() int
	SetDeviceId(int)
	NumOfUsers() int
	AddUser(User)
	RemoveUser(User)
	IsActiveUser(User) bool
	GetPackets(User) []CanData
}

type CanData struct {
	Src string	// Where packet originated
	SeqNo int
	AbsTime string
	RelTime string
	Status string
	Error string
	Transmit string
	Desc string
	Network string
	Node string
	ArbID string
	Remote bool
	Extended bool
	B1 uint8
	B2 uint8
	B3 uint8
	B4 uint8
	B5 uint8
	B6 uint8
	B7 uint8
	B8 uint8
	Value string
	Trigger string
	Signals string
}


type CanibusAPIVersion struct {
	Major int
	Minor int
	Sub   int
}

func (c *CanibusAPIVersion) ToString() string {
	return fmt.Sprintf("%d.%d.%d", c.Major, c.Minor, c.Sub)
}

type Server struct {
	Version string
}

// Cmd Structure of client issuing commands to server
type Cmd struct {
	Action string
	Arg    []string
}

// Generic Error struct to send clients
type Err struct {
	Type string
	Msg string
}

type Client struct {
	ClientID int
	Cookie   string
}

type Msg struct {
	Type     string
	ClientID int
	Author   string
	Value    string
}

var APIVersion CanibusAPIVersion
var ServerVersion Server

func InitAPI() {
	APIVersion.Major = 0
	APIVersion.Minor = 0
	APIVersion.Sub = 2
	ServerVersion.Version = APIVersion.ToString()
}

func ProcessLogin(cmd *Cmd) error {
        switch len(cmd.Arg) {
        case 1:
                if cmd.Arg[0] == "" {
                        return logger.Err("Login: Invalid Name")
                }
		// Currently no real checks
		return nil
        default:
                return logger.Err("Login: Wrong number of arguments needed to login")
        }
	return logger.Err("Internal error for ProcessLogin")
}
