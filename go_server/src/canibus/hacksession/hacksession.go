package hacksession

import (
	"canibus/canibususer"
	"fmt"
)

const (
	STATE_IDLE = iota
	STATE_CONFIG
	STATE_SNIFF
)

type HackSession struct {
	Users []canibususer.CanibusUser
	State int
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

