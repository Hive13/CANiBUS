// Base CanDevice Interface
package candevice

import (
	"canibus/hacksession"
)

type CanDevice interface {
	Init() bool
	DeviceType() string
	DeviceDesc() string
	GetId() int
	SetId(int)
	GetHackSession() *hacksession.HackSession
	SetHackSession(hacksession.HackSession)
}

