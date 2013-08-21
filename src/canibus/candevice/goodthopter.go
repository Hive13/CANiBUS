package candevice

import (
	"canibus/logger"
	"fmt"
	"github.com/tarm/goserial"
	"io"
)

type GoodThopter struct {
	SerialPort string
	Serial     io.ReadWriteCloser
	App        uint8
	Verb       uint8
}

func (mcp *GoodThopter) SetSerial(port string) {
	mcp.SerialPort = port
}

func (mcp *GoodThopter) DeviceDesc() string {
	return "TODO: VIN"
}

func (mcp *GoodThopter) DeviceType() string {
	return "GoodThopter"
}

func (mcp *GoodThopter) Init() bool {
	c := &serial.Config{Name: "/dev/ttyUSB0", Baud: 115200}
	c.Name = mcp.SerialPort
	s, err := serial.OpenPort(c)
	if err != nil {
		logger.Log("Could not init GoodThopter")
		return false
	}
	mcp.Serial = s
	// setRTS(1)
	// setDTR(1)
	// setDTR(0)
	mcp.readCmd()
	if mcp.Verb != 0x7f {
		logger.Log("TODO: didn't get proper verb. retry")
		return false
	}
	// infostring()
	// monitorclocking()
	// mon_connected()
	return true
}

func (mcp *GoodThopter) readCmd() {
	mcp.App = mcp.readByte()
	mcp.Verb = mcp.readByte()
}

// TODO verify the data type, probably an array
func (mcp *GoodThopter) writeCmd(app uint8, verb uint8, count uint16, data uint8) {
	mcp.writeByte(app)
	mcp.writeByte(verb)
	mcp.writeByte((uint8)(count & 0xff))
	mcp.writeByte((uint8)(count >> 8))
	// TODO use count for data as size of data
	mcp.writeByte(data)
}

func (mcp *GoodThopter) infostring() string {
	a := mcp.monPeek8(0xf0)
	b := mcp.monPeek8(0xf1)
	// TODO: convert return to a string
	return fmt.Sprintf("%02x%02x", a, b)
}

func (mcp *GoodThopter) readByte() uint8 {
	buf := make([]byte, 128)
	_, err := mcp.Serial.Read(buf)
	if err != nil {
		logger.Err("Could not read from Thopter")
		return 0
	}
	return buf[0]
}

func (mcp *GoodThopter) writeByte(b uint8) {
	data := make([]byte, b)
	_, err := mcp.Serial.Write(data)
	if err != nil {
		logger.Err("Could not write to Thopter")
	}
}

func (mcp *GoodThopter) monPeek8(address uint8) uint8 {
	mcp.writeCmd(0, 0x02, 2, address)
	return address // TODO double check this function
}
