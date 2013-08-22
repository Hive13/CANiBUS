package hacksession

import (
	"canibus/api"
	"canibus/logger"
	"fmt"
)

const (
	STATE_IDLE = iota
	STATE_CONFIG
	STATE_SNIFF
)

type HackSession struct {
	Users    []api.User
	State    int
	DeviceId int
	Device   api.CanDevice
}

func (s *HackSession) GetState() string {
	if s.State == STATE_CONFIG {
		return "Config..."
	} else if s.State == STATE_SNIFF {
		return fmt.Sprintf("Active: %d", len(s.Users))
	}
	return "Idle"
}

func (s *HackSession) GetStateValue() int {
	return s.State
}

func (s *HackSession) SetState(state int) {
	s.State = state
}

func (s *HackSession) GetDeviceId() int {
	return s.DeviceId
}

func (s *HackSession) SetDeviceId(id int) {
	s.DeviceId = id
}

func (s *HackSession) SetDevice(dev api.CanDevice) {
	s.Device = dev
	s.DeviceId = dev.GetId()
}

func (s *HackSession) NumOfUsers() int {
	return len(s.Users)
}

func (s *HackSession) AddUser(user api.User) {
	s.Users = append(s.Users, user)
}

func (s *HackSession) RemoveUser(user api.User) {
	var newUsers []api.User
	for i := range s.Users {
		if s.Users[i].GetName() != user.GetName() {
			newUsers = append(newUsers, s.Users[i])
		}
	}
	s.Users = newUsers
}

func (s *HackSession) IsActiveUser(user api.User) bool {
	for i := range s.Users {
		if s.Users[i].GetName() == user.GetName() {
			return true
		}
	}
	return false
}

func (s *HackSession) GetPackets(user api.User) []api.CanData {
	var pkts []api.CanData
	var idx int
	if s.Device == nil {
		return pkts
	}
	if user.LastIdx() == 0 { // First time sniffing
		user.SetLastIdx(s.Device.GetPacketIdx())
	}
	pkts, idx = s.Device.GetPacketsFrom(user.LastIdx())
	// TODO: Apply filters
	user.SetLastIdx(idx)
	return pkts
}

func (s *HackSession) InjectPacket(user api.User, TxPkt api.TransmitPacket) error {
	if s.Device == nil {
		return logger.Err("Device not set")
	}
	var err error
	pkt := api.CanData{}
	pkt.Src = user.GetName()
	pkt.ArbID = TxPkt.ArbId
	pkt.Network = TxPkt.Network
	pkt.B1, err = api.Atoui8(TxPkt.B1)
	if err != nil {
		return err
	}
	pkt.B2, err = api.Atoui8(TxPkt.B2)
	if err != nil {
		return err
	}
	pkt.B3, err = api.Atoui8(TxPkt.B3)
	if err != nil {
		return err
	}
	pkt.B4, err = api.Atoui8(TxPkt.B4)
	if err != nil {
		return err
	}
	pkt.B5, err = api.Atoui8(TxPkt.B5)
	if err != nil {
		return err
	}
	pkt.B6, err = api.Atoui8(TxPkt.B6)
	if err != nil {
		return err
	}
	pkt.B7, err = api.Atoui8(TxPkt.B7)
	if err != nil {
		return err
	}
	pkt.B8, err = api.Atoui8(TxPkt.B8)
	if err != nil {
		return err
	}
	err = s.Device.InjectPacket(pkt)
	return err
}
