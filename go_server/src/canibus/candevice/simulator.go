package candevice

import (
	"canibus/api"
	"canibus/logger"
	"encoding/json"
	"io/ioutil"
)

type Simulator struct {
	PacketFile string
	Packets []CanData
	HackSession api.HackSession
	id int
}

func (sim *Simulator) SetPacketFile(packets string) {
	sim.PacketFile = packets
}

func (sim *Simulator) Init() bool {
	logger.Log("Loading packets from " + sim.PacketFile)
	err := sim.LoadCanDataFromFile(sim.PacketFile)
	if err != nil {
		return false
	}
	return true
}

func (sim *Simulator) LoadCanDataFromFile(file string) error {
	packets, err := ioutil.ReadFile(sim.PacketFile)
	if err != nil {
		logger.Log("Could not open Simulator data file")
		return err
	}
	var canPacket []CanData
	err = json.Unmarshal(packets, &canPacket)
	if err != nil {
		logger.Log("Problem with json unmarshal sim data")
		return err
	} else {
		sim.Packets = canPacket
	}
	return nil
}

func (sim *Simulator) DeviceDesc() string {
	return sim.PacketFile
}

func (sim *Simulator) DeviceType() string {
	return "Simulator"
}

func (sim *Simulator) GetHackSession() api.HackSession {
	return sim.HackSession
}

func (sim *Simulator) SetHackSession(hsession api.HackSession) {
	sim.HackSession = hsession
}

func (sim *Simulator) GetId() int {
	return sim.id
}

func (sim *Simulator) SetId(id int) {
	sim.id = id
}

func (sim *Simulator) GetYear() string {
	return ""
}

func (sim *Simulator) GetMake() string {
	return ""
}

func (sim *Simulator) GetModel() string {
	return ""
}
