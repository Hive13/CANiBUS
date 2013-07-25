package candevice

type CanData struct {
	SeqNo int
	AbsTime string
	RelTime string
	Status string
	Error string
	Transmit string
	Desc string
	Network string
	Node string
	ArbID string
	Remote bool
	Extended bool
	B1 uint8
	B2 uint8
	B3 uint8
	B4 uint8
	B5 uint8
	B6 uint8
	B7 uint8
	B8 uint8
	Value string
	Trigger string
	Signals string
}
