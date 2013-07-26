package server

import (
)

func InitDrivers(conf Config) {
	for i := range conf.Drivers {
		conf.Drivers[i].Init()
	}
}
