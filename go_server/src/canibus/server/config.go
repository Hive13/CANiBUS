package server

import (
	"canibus/api"
	"canibus/candevice"
	"canibus/logger"
	"encoding/json"
	"os"
	"io"
	"fmt"
)

type ConfigElement struct {
	DeviceType string
	DeviceFile string
	DeviceSerial string
}

type Config struct {
	Drivers []api.CanDevice
}

func (c *Config) LoadConfig(conf string) {
	if len(conf) == 0 {
		logger.Log("No config file given")
		return
	}
	cfile, err := os.Open(conf)
	if err != nil {
		logger.Log("Could not open config file")
		return
	}
	var elem []ConfigElement
	dec := json.NewDecoder(cfile)
	for {
		err = dec.Decode(&elem)
		if err != nil && err != io.EOF {
			fmt.Println("Error:", err)
			logger.Log("Could not decode config element")
		} else {
			if err == io.EOF {
				break
			}
			for i := range elem {
				if elem[i].DeviceType == "simulator" {
					dev := &candevice.Simulator{}
					dev.SetPacketFile(elem[i].DeviceFile)
					c.appendDriver(dev)
				} else {
					fmt.Printf("Unknown config setting: %+v\n", elem[i])
				}
			}
		}
	}
	cfile.Close()
}

func (c *Config) appendDriver(drv api.CanDevice) {
	var next_id int
	next_id = 0
	for i:= range c.Drivers {
		if next_id <= c.Drivers[i].GetId() {
			next_id = c.Drivers[i].GetId()
		}
	}
	next_id += 1
	drv.SetId(next_id)
	c.Drivers = append(c.Drivers, drv)
}

func (c *Config) GetDrivers() []api.CanDevice {
	return c.Drivers
}
