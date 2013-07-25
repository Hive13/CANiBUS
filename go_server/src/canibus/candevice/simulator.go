package candevice

import (
	"canibus/logger"
	"encoding/json"
	"os"
	"io"
)

type Simulator struct {
	PacketFile string
	Packets []CanData
}

func (sim *Simulator) setPacketFile(packets string) {
	sim.PacketFile = packets
}

func (sim *Simulator) Init() bool {
	packets, err := os.Open(sim.PacketFile)
	if err != nil {
		logger.Log("Could not open Simulator data file")
		return false
	}
	buf := make([]byte, 1024)
	var canPacket []CanData
	for {
		n, err := packets.Read(buf)
		if err != nil && err != io.EOF {
			logger.Log("Could not read Simulator data file")
			return false
		}
		if n == 0 {
			break
		}
		err = json.Unmarshal(buf, &canPacket)
		if err != nil {
			logger.Log("Problem with json unmarshal sim data")
		} else {
			sim.Packets = append(sim.Packets, canPacket...)
		}
	}
	packets.Close()
	return true
}
