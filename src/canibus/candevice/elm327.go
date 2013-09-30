package candevice

import (
	"canibus/api"
	"canibus/logger"
	"canibus/obd"
	"canibus/serialbuffer"
	"encoding/hex"
	"fmt"
	"time"
	"strings"
)

type Elm327 struct {
	Serial            serialbuffer.SerialBuffer
	Type              string
	Desc              string
	VIN               string
	id                int
	Year              string
	Make              string
	VehicleAttributes obd.VehicleAttributes
	HackSession       api.HackSession
	Packets           [MAX_BUFFER]api.CanData
	packetIdx         int
	sniffEnabled      bool
	seqNo             int
}

func (e *Elm327) SetSerial(port string) {
	e.Serial.SetSerial(port)
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
	logger.Log("Initializing ELM327 on " + e.Serial.SerialPort)
	e.Type = "ELM327"
	e.Desc = "Not connected"
	e.Serial.SetBaud(9600)
	ok := e.Serial.Init()
	if !ok {
		logger.Log("Could not open ELM327 device")
		return false
	}
	e.Serial.SetPromptChar(62) // >
	var resp []string
	fmt.Println("Clearing buffers")
	//e.Serial.ReadLn() // Clear buffer
	fmt.Println("Sending Reset")
	resp, _ = e.SendCmd("ATI")
	fmt.Println("DEBUG: resp=", resp)
	e.Type = resp[0]
	fmt.Println("DEBUG: Type=",e.Type)
	fmt.Println("Turning off echo")
	e.SendCmd("ATE0")
	e.SendCmd("ATH0")
	vin := e.GetVIN()
	fmt.Println("got vin: ", vin)
	e.Desc = "VIN: " + vin
	e.Year = obd.GetYearFromVIN(vin)
	e.Make = obd.GetMakeFromVIN(vin)
	e.VehicleAttributes = obd.GetModelFromVIN(vin)
	e.SendCmd("ATH1")
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
	cmdResp, vin_err := e.SendCmd("09 02")
	if vin_err != nil {
		return vin_err.Error()
	}
	// TODO do not hard code this :)
	vin := make([]byte, 34)
	fmt.Println("DEBUG: cmdResp1", cmdResp[1])
	fmt.Println("DEBUG: cmdResp2", cmdResp[2])
	fmt.Println("DEBUG: cmdResp3", cmdResp[3])
	resp := []byte(cmdResp[1])
	vin[0] = resp[12]
	vin[1] = resp[13]
	vin[2] = resp[15]
	vin[3] = resp[16]
	vin[4] = resp[18]
	vin[5] = resp[19]
	resp = []byte(cmdResp[2])
	vin[6] = resp[3]
	vin[7] = resp[4]
	vin[8] = resp[6]
	vin[9] = resp[7]
	vin[10] = resp[9]
	vin[11] = resp[10]
	vin[12] = resp[12]
	vin[13] = resp[13]
	vin[14] = resp[15]
	vin[15] = resp[16]
	vin[16] = resp[18]
	vin[17] = resp[19]
	vin[18] = resp[21]
	vin[19] = resp[22]
	resp = []byte(cmdResp[3])
	vin[20] = resp[3]
	vin[21] = resp[4]
	vin[22] = resp[6]
	vin[23] = resp[7]
	vin[24] = resp[9]
	vin[25] = resp[10]
	vin[26] = resp[12]
	vin[27] = resp[13]
	vin[28] = resp[15]
	vin[29] = resp[16]
	vin[30] = resp[18]
	vin[31] = resp[19]
	vin[32] = resp[21]
	vin[33] = resp[22]
	vinAscii := make([]byte, 17)
	hex.Decode(vinAscii, vin)
	e.VIN = string(vinAscii)
	return string(vinAscii)
}

// Sends a command then polls until it recieves a prompt
func (e *Elm327) SendCmd(line string) ([]string, error) {
	fmt.Println("DEBUG: Sending command: ", line)
	e.Serial.Writeln(line)
	timeout := 10
	maxlines := 5
	var result []string
	done := false
	for timeout > 0 && !done {
		timeout -= 1
		resp, gotLine := e.Serial.ReadLn()
		fmt.Println("DEBUG: gotLine",gotLine)
		if gotLine {
			maxlines -= 1
		} else {
			timeout -= 1
			if e.Serial.GotPrompt {
				done = true
			}
		}
		if maxlines == 0 {
			done = true
		}
		// Ignore ECHO
		if line != resp {
			fmt.Println("DEBUG: SendCmd resp=",resp)
			if len(resp) > 1 {
				result = append(result, string(resp))
			}
		}
	}
	fmt.Println("SendCmd Results: ", result)
	if len(result) > 1 && strings.Contains(result[1],"UNABLE TO CONNECT") {
		return nil, logger.Err("Unable to Connect to ECU")
	}
	return result, nil
}

func (e *Elm327) Writeln(line string) {
	line = line + "\r"
	err := e.Serial.Write([]byte(line))
	if err != nil {
		logger.Log("Could not write to ELM327")
	}
}

func (e *Elm327) StartSniffing() {
	e.sniffEnabled = true
	e.packetIdx = 0
	e.seqNo = 0
	go e.processPackets()
}

func (e *Elm327) StopSniffing() {
	e.sniffEnabled = false
}

func (e *Elm327) parsePacket(data string) api.CanData {
	b := []byte(data)
	var p string
	gotArbId := false
	gotB1 := false
	gotB2 := false
	gotB3 := false
	gotB4 := false
	gotB5 := false
	gotB6 := false
	gotB7 := false
	gotB8 := false
	pkt := api.CanData{}
	idx := 0
	done := false
	for done == false && idx < len(b) {
		if b[idx] == 60 { // < char
			done = true
		} else if !gotArbId {
			if b[idx] == 32 {
				gotArbId = true
				pkt.ArbID = p
				p = ""
			} else {
				p = p + string(b[idx])
			}
		} else if !gotB1 {
			if b[idx] == 32 || b[idx] == 13 {
				gotB1 = true
				pkt.B1, _ = api.Hextoui8(p)
				p = ""
				if b[idx] == 13 {
					done = true
				}
			} else {
				p = p + string(b[idx])
			}
		} else if !gotB2 {
			if b[idx] == 32 || b[idx] == 13 {
				gotB2 = true
				pkt.B2, _ = api.Hextoui8(p)
				p = ""
				if b[idx] == 13 {
					done = true
				}
			} else {
				p = p + string(b[idx])
			}
		} else if !gotB3 {
			if b[idx] == 32 || b[idx] == 13 {
				gotB3 = true
				pkt.B3, _ = api.Hextoui8(p)
				p = ""
				if b[idx] == 13 {
					done = true
				}
			} else {
				p = p + string(b[idx])
			}
		} else if !gotB4 {
			if b[idx] == 32 || b[idx] == 13 {
				gotB4 = true
				pkt.B4, _ = api.Hextoui8(p)
				p = ""
				if b[idx] == 13 {
					done = true
				}
			} else {
				p = p + string(b[idx])
			}
		} else if !gotB5 {
			if b[idx] == 32 || b[idx] == 13 {
				gotB5 = true
				pkt.B5, _ = api.Hextoui8(p)
				p = ""
				if b[idx] == 13 {
					done = true
				}
			} else {
				p = p + string(b[idx])
			}
		} else if !gotB6 {
			if b[idx] == 32 || b[idx] == 13 {
				gotB6 = true
				pkt.B6, _ = api.Hextoui8(p)
				p = ""
				if b[idx] == 13 {
					done = true
				}
			} else {
				p = p + string(b[idx])
			}
		} else if !gotB7 {
			if b[idx] == 32 || b[idx] == 13 {
				gotB7 = true
				pkt.B7, _ = api.Hextoui8(p)
				p = ""
				if b[idx] == 13 {
					done = true
				}
			} else {
				p = p + string(b[idx])
			}

		} else if !gotB8 {
			if b[idx] == 32 || b[idx] == 13 {
				gotB8 = true
				pkt.B8, _ = api.Hextoui8(p)
				done = true
			} else {
				p = p + string(b[idx])
			}
		}
		idx += 1
	}
	return pkt
}

func (e *Elm327) processPackets() {
	sniffing := false
	var resp string
	for e.sniffEnabled == true {
		if !sniffing {
			//fmt.Println("Writing ATMA")
			e.Writeln("ATMA")
			sniffing = true
		} else {
			//fmt.Println("BufReadln()")
			resp, _ = e.Serial.ReadLn()
			if len(resp) > 1 && resp[0] == e.Serial.GetPromptChar() {
				resp = resp[1:]
			}
			//fmt.Println("Reading:",resp);
			if resp == "SEARCHING..." ||
			   resp == "" ||
                           []byte(resp)[0] == 13 {
				// do nothing
			} else if strings.Contains(resp, "BUFFER FULL") {
				sniffing = false
			} else { // Attempt to parse packet
				pkt := e.parsePacket(resp)
				if pkt.ArbID != "" {
					pkt.Src = "Elm327"
					e.addPacket(pkt)
				}
			}
		}
	}
}

func (e *Elm327) addPacket(canpkt api.CanData) {
	pkt := api.CanData{}
	pkt.SeqNo = e.seqNo
	e.seqNo += 1
	pkt.Src = canpkt.Src
	pkt.AbsTime = time.Now().Format("10:00:00pm (EST)")
	pkt.RelTime = "TODO" // TODO
	pkt.Status = canpkt.Status
	pkt.Error = canpkt.Error
	pkt.Transmit = canpkt.Transmit
	pkt.Desc = canpkt.Desc
	pkt.Network = canpkt.Network
	pkt.Node = canpkt.Node
	pkt.ArbID = canpkt.ArbID
	pkt.Remote = canpkt.Remote
	pkt.Extended = canpkt.Extended
	pkt.B1 = canpkt.B1
	pkt.B2 = canpkt.B2
	pkt.B3 = canpkt.B3
	pkt.B4 = canpkt.B4
	pkt.B5 = canpkt.B5
	pkt.B6 = canpkt.B6
	pkt.B7 = canpkt.B7
	pkt.B8 = canpkt.B8
	pkt.Value = canpkt.Value
	pkt.Trigger = canpkt.Trigger
	e.Packets[e.packetIdx] = pkt
	e.packetIdx += 1
	if e.packetIdx >= MAX_BUFFER {
		e.packetIdx = 0
	}
}

func (e *Elm327) GetPacketsFrom(idx int) ([]api.CanData, int) {
	var pkts []api.CanData
	done := false
	appends := 0
	for done != true {
		if idx >= MAX_BUFFER || idx < 0 {
			idx = 0
		}
		if idx == e.packetIdx {
			return pkts, e.packetIdx
		}
		pkts = append(pkts, e.Packets[idx])
		idx += 1
		appends += 1
		if appends > MAX_APPENDS {
			done = true
		}
	}
	return pkts, e.packetIdx
}

func (e *Elm327) GetPacketIdx() int {
	return e.packetIdx
}

func (e *Elm327) InjectPacket(pkt api.CanData) error {
	// TODO the actual sending of the packet to the candevice
	e.addPacket(pkt)
	return nil
}

