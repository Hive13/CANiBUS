package canibususer

import (

)

type CanibusUser struct {
	Name string
	DeviceId int
	lastIdx int // Index values for packet sequences
}

func (u *CanibusUser) GetName() string {
	return u.Name
}

func (u *CanibusUser) SetName(name string) {
	u.Name = name
}

func (u *CanibusUser) GetDeviceId() int {
	return u.DeviceId
}

func (u *CanibusUser) SetDeviceId(id int) {
	u.DeviceId = id
}

func (u *CanibusUser) LastIdx() int {
	return u.lastIdx
}

func (u *CanibusUser) SetLastIdx(idx int) {
	u.lastIdx = idx
}
