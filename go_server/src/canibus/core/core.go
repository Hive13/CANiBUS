// Core memory handling package for CANiBUS
package core

import (
	"canibus/api"
	"canibus/logger"
)

type CoreData struct {
	CConfig api.Configer
	Users []api.User
}

var CData CoreData

func SetConfig(conf api.Configer) {
	CData.CConfig = conf
}

func GetConfig() api.Configer {
	return CData.CConfig
}

func LoadConfig(confFile string) {
	var conf = CData.CConfig
	conf.LoadConfig(confFile)
}

func NumberOfUsers() int {
	if CData.Users == nil {
		return 0
	}
	return len(CData.Users)
}

func GetUserByName(userName string) (*api.User, error) {
	if CData.Users == nil {
		return nil, logger.Err("No registered users")
	}
	for i := range CData.Users {
		if (CData.Users[i].GetName() == userName) {
			return &CData.Users[i], nil
		}
	}
	return nil, logger.Err("User not found")
}

func AddUser(user api.User) {
	CData.Users = append(CData.Users, user)
}
