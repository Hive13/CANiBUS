package candevice

import (
	"canibus/api"
	"canibus/logger"
	"encoding/json"
	"io/ioutil"
	//	"strconv"
	"time"
)

const (
	MAX_BUFFER  = 10000 // Packets in buffer
	MAX_APPENDS = 1000  // Max packets returned at one time
)

type Simulator struct {
	PacketFile   string
	SimPackets   []api.CanData
	Packets      [MAX_BUFFER]api.CanData
	HackSession  api.HackSession
	id           int
	sniffEnabled bool
	packetIdx    int
	seqNo        int
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
	var canPacket []api.CanData
	err = json.Unmarshal(packets, &canPacket)
	if err != nil {
		logger.Log("Problem with json unmarshal sim data")
		return err
	} else {
		sim.SimPackets = canPacket
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

func (sim *Simulator) StartSniffing() {
	sim.sniffEnabled = true
	sim.packetIdx = 0
	sim.seqNo = 0
	go sim.processPackets()
}

func (sim *Simulator) StopSniffing() {
	sim.sniffEnabled = false
}

func (sim *Simulator) addPacket(simPkt api.CanData) {
	pkt := api.CanData{}
	pkt.SeqNo = sim.seqNo
	sim.seqNo += 1
	pkt.Src = simPkt.Src
	pkt.AbsTime = time.Now().Format("10:00:00pm (EST)")
	pkt.RelTime = simPkt.RelTime
	pkt.Status = simPkt.Status
	pkt.Error = simPkt.Error
	pkt.Transmit = simPkt.Transmit
	pkt.Desc = simPkt.Desc
	pkt.Network = simPkt.Network
	pkt.Node = simPkt.Node
	pkt.ArbID = simPkt.ArbID
	pkt.Remote = simPkt.Remote
	pkt.Extended = simPkt.Extended
	pkt.B1 = simPkt.B1
	pkt.B2 = simPkt.B2
	pkt.B3 = simPkt.B3
	pkt.B4 = simPkt.B4
	pkt.B5 = simPkt.B5
	pkt.B6 = simPkt.B6
	pkt.B7 = simPkt.B7
	pkt.B8 = simPkt.B8
	pkt.Value = simPkt.Value
	pkt.Trigger = simPkt.Trigger
	pkt.Signals = simPkt.Signals
	sim.Packets[sim.packetIdx] = pkt
	sim.packetIdx += 1
	if sim.packetIdx >= MAX_BUFFER {
		sim.packetIdx = 0
	}
}

func (sim *Simulator) processPackets() {
	total := len(sim.SimPackets)
	simIdx := 0
	//startedAt := time.Now()
	// ported from old canibusd
	//relDiff := float64(startedAt.Second() + (startedAt.Nanosecond() / 1000000000.0))
	for sim.sniffEnabled == true {
		if simIdx >= total || simIdx < 0 {
			simIdx = 0
		}
		/*
			pktRelTime, cerr := strconv.ParseFloat(sim.SimPackets[simIdx].RelTime, 32)
			if cerr == nil {
				if sim.SimPackets[simIdx].SeqNo != 0 && relDiff > pktRelTime {
					relDiff -= pktRelTime
					sim.addPacket(sim.SimPackets[simIdx])
					simIdx += 1
				}
			}
		*/
		// FIXME Just temp code to test interface
		sim.SimPackets[simIdx].Src = "Sim"
		sim.addPacket(sim.SimPackets[simIdx])
		simIdx += 1
		time.Sleep(100 * time.Millisecond)
	}
}

func (sim *Simulator) GetPacketsFrom(idx int) ([]api.CanData, int) {
	var pkts []api.CanData
	var done bool
	done = false
	appends := 0
	for done != true {
		if idx > MAX_BUFFER || idx < 0 {
			idx = 0
		}
		if idx == sim.packetIdx {
			return pkts, sim.packetIdx
		}
		pkts = append(pkts, sim.Packets[idx])
		idx += 1
		appends += 1
		if appends > MAX_APPENDS {
			done = true
		}
	}
	// We only get here if we appended more than MAX_APPENDS
	return pkts, sim.packetIdx
}

func (sim *Simulator) GetPacketIdx() int {
	return sim.packetIdx
}
