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
	buf := make([]byte, 1024)
	var elem []ConfigElement
	for {
		n, err := cfile.Read(buf)
		if err != nil && err != io.EOF {
			logger.Log("Could not read config file")
			return
		}
		if n == 0 {
			break
		}
		err = json.Unmarshal(buf, &elem)
		if err != nil {
			fmt.Println("Error:", err)
			logger.Log("Could not unmarshal config element")
		} else {
			fmt.Printf("DEBUG: %+v", elem)
		}
	}
	cfile.Close()
}
