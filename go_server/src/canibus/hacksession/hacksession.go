package hacksession

import (
	"canibus/api"
	"fmt"
)

const (
	STATE_IDLE = iota
	STATE_CONFIG
	STATE_SNIFF
)

type HackSession struct {
	Users []api.User
	State int
	DeviceId int
}

func (s *HackSession)GetState() string {
	if s.State == STATE_CONFIG {
		return "Config..."
	} else if s.State == STATE_SNIFF {
		return fmt.Sprintf("Active: %d", len(s.Users))
	}
	return "Idle"
}

func (s *HackSession)SetState(state int) {
	s.State = state
}

func (s *HackSession)GetDeviceId() int {
	return s.DeviceId
}

func (s *HackSession)SetDeviceId(id int) {
	s.DeviceId = id
}

func (s *HackSession)NumOfUsers() int {
	return len(s.Users)
}

func (s *HackSession)AddUser(user api.User) {
	s.Users = append(s.Users, user)
}

func (s *HackSession)RemoveUser(user api.User) {
	var newUsers []api.User
	for i := range s.Users {
		if s.Users[i].GetName() != user.GetName() {
			newUsers = append(newUsers, s.Users[i])
		}
	}
	s.Users = newUsers
}

