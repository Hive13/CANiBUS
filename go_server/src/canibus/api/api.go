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
}

type HackSession interface {
	GetState() string
	SetState(int)
	GetDeviceId() int
	SetDeviceId(int)
	NumOfUsers() int
	AddUser(User)
	RemoveUser(User)
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
