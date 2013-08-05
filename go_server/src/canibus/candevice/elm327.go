package candevice

import (
	"canibus/api"
	"canibus/logger"
	"canibus/obd"
	"github.com/tarm/goserial"
	"encoding/hex"
	"io"
	"fmt"
)

type Elm327 struct {
	SerialPort string
	Serial io.ReadWriteCloser
	Type string
	Desc string
	VIN string
	id int
	Year string
	Make string
	VehicleAttributes obd.VehicleAttributes
	HackSession api.HackSession
}

func (e *Elm327) SetSerial(port string) {
	e.SerialPort = port
}

func (e *Elm327) GetYear() string {
	return e.Year
}

func (e *Elm327) GetMake() string {
	return e.Make
}

func (e *Elm327) GetModel() string {
	return e.VehicleAttributes.Model
}

func (e *Elm327) Init() bool {
	logger.Log("Initializing ELM327 on " + e.SerialPort)
	e.Type = "ELM327"
	e.Desc = "Not connected"
	c := &serial.Config{Name: e.SerialPort, Baud: 9600}
	s, err := serial.OpenPort(c)
	if err != nil {
		logger.Log("Could not open ELM327 device")
		return false
	}
	e.Serial = s
	resp, _ := e.Readln()  // Clear buffer
	resp, err = e.SendCmd("ATZ")
	e.Type = string(resp[1:]) // Skip first \r
	resp, _ = e.SendCmd("ATE0")
	vin := e.GetVIN()
	fmt.Println("got vin: ", vin)
	e.Desc = "VIN: " + vin
	e.Year = obd.GetYearFromVIN(vin)
	e.Make = obd.GetMakeFromVIN(vin)
	e.VehicleAttributes = obd.GetModelFromVIN(vin)
	return true
}

func (e *Elm327) DeviceType() string {
	return e.Type
}

func (e *Elm327) DeviceDesc() string {
	return e.Desc
}

func (e *Elm327) GetId() int {
	return e.id
}

func (e *Elm327) SetId(id int) {
	e.id = id
}

func (e *Elm327) GetHackSession() api.HackSession {
	return e.HackSession
}

func (e *Elm327) SetHackSession(hax api.HackSession) {
	e.HackSession = hax
}

func (e *Elm327) GetVIN() string {
	if e.VIN != "" {
		return e.VIN
	}
	resp, _ := e.SendCmd("09 02")
/*
	respLines := e.toStrings(resp)
	for i := range respLines {
		fmt.Println("VIN: ", respLines[i])
	}
	for i := range resp {
		fmt.Print("[", i, "]", resp[i], " ")
	}
	fmt.Println("")
	vinSize := respLines[0]
*/
	// TODO do not hard code this :)
	vin := make([]byte, 34)
	vin[0] = resp[17]
	vin[1] = resp[18]
	vin[2] = resp[20]
	vin[3] = resp[21]
	vin[4] = resp[23]
	vin[5] = resp[24]
	vin[6] = resp[30]
	vin[7] = resp[31]
	vin[8] = resp[33]
	vin[9] = resp[34]
	vin[10] = resp[36]
	vin[11] = resp[37]
	vin[12] = resp[39]
	vin[13] = resp[40]
	vin[14] = resp[42]
	vin[15] = resp[43]
	vin[16] = resp[45]
	vin[17] = resp[46]
	vin[18] = resp[48]
	vin[19] = resp[49]
	vin[20] = resp[55]
	vin[21] = resp[56]
	vin[22] = resp[58]
	vin[23] = resp[59]
	vin[24] = resp[61]
	vin[25] = resp[62]
	vin[26] = resp[64]
	vin[27] = resp[65]
	vin[28] = resp[67]
	vin[29] = resp[68]
	vin[30] = resp[70]
	vin[31] = resp[71]
	vin[32] = resp[73]
	vin[33] = resp[74]
	vinAscii := make([]byte, 17)
	hex.Decode(vinAscii, vin)
	e.VIN = string(vinAscii)
	return string(vinAscii)
}

func (e *Elm327) SendCmd(line string) ([]byte, error) {
	fmt.Println("DEBUG: Sending command: ", line)
	e.Writeln(line)
	timeout := 100
	maxlines := 5
	var result []byte
	done := false
	for timeout > 0 && !done {
		timeout -= 1
		resp, gotPrompt := e.Readln()
		maxlines -= 1
		if maxlines == 0 {
			done = true
		}
		if gotPrompt && len(resp) > 0 {
			done = true
		}
		// Ignore ECHO
		if line != string(resp) {
			//fmt.Println("DEBUG: SendCmd resp=",resp)
			if len(resp) > 1 && resp[len(resp)-1] == 63 {
				done = true // Error
			} else {
				result = append(result, resp...)
			}
		}
		if len(resp) == 0 && len(result) > 0 {
			done = true
		}
		if len(result) > 0 {
			fmt.Println("SendCmd: %#v ", resp, result)
		}
	}
	fmt.Println("SendCmd Results: ", e.toStrings(result))
	return result, nil
}

func (e *Elm327) Writeln(line string) {
	line = line + "\r"
	_, err := e.Serial.Write([]byte(line))
	if err != nil {
		logger.Log("Could not write to ELM327")
	}
}

func (e *Elm327) Read() []byte {
	buf := make([]byte, 128)
	n, err := e.Serial.Read(buf)
	if err != nil {
		logger.Log("Could not read from ELM327")
		return nil
	}
	return buf[:n]
}

func (e *Elm327) Readln() ([]byte, bool) {
	timeout := 1000
	var result []byte
	done := false
	for timeout > 0 && !done {
		timeout -= 1
		resp := e.Read()
		//fmt.Println("Readln resp = ", resp)
		if len(resp) > 3 {
			if resp[0] == 63 && resp[1] == 13 && resp[2] == 13 && resp[3] == 62 {
				if len(resp) > 4 {
					return []byte(resp[4:]), true
				} else {
					return nil, true
				}
			}
			if resp[len(resp)-1] == 62 && resp[len(resp)-2] == 13 && resp[len(resp)-3] == 13 {
				return []byte(resp[:len(resp)-3]), true
			}
		}
		if resp[len(resp)-1] == 13 {
			result = append(result, []byte(resp[:len(resp)-1])...)
			return result, false
		}
		result = append(result, resp...)
		//fmt.Println("Readln: %#v : %s", result, string(result))
	}
	return nil, false
}

func (e *Elm327) toStrings(pkt []byte) []string {
	var start = 0
	var lines []string
	for i := range pkt {
		if pkt[i] == 13 {
			//fmt.Println("DEBUG toStrings: start, i", start, i)
			if start != i {
				lines = append(lines, string(pkt[start:i-1]))
				start = i + 1
			}
		}
	}
	lines = append(lines, string(pkt[start:]))
	return lines
}
