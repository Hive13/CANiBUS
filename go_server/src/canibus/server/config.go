package server

import (
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
}

func (c *Config) LoadConfig(conf string) {
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
			fmt.Printf("DEBUG: %+v\n", elem)
		}
	}
	cfile.Close()
}
