// Core memory handling package for CANiBUS
package core

import (
	"canibus/api"
)

type CoreData struct {
	CConfig api.Configer
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
