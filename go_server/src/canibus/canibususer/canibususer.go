package canibususer

import (

)

type CanibusUser struct {
	Name string
}

func (u *CanibusUser) GetName() string {
	return u.Name
}

func (u *CanibusUser) SetName(name string) {
	u.Name = name
}
