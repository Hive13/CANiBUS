package serialbuffer

import (
	"github.com/tarm/goserial"
	"bufio"
	"io"
	"time"
	"fmt"
)

const (
	MAX_SERIAL_BUF = 1000
	MAX_LINE_BUF = 100
)

type SerialBuffer struct {
	SerialPort	string
	Baud		int
	Serial		io.ReadWriteCloser
	Scanner		*bufio.Scanner
	Lines		[MAX_LINE_BUF]string
	TmpBuf		[]byte
	BufPtr		int
	CurPtr		int
	PromptChar	byte
	GotPrompt	bool
}

func (s *SerialBuffer) SetSerial(port string) {
	s.SerialPort = port
}

func (s *SerialBuffer) GetSerial() string {
	return s.SerialPort
}

func (s *SerialBuffer) SetBaud(baud int) {
	s.Baud = baud
}

func (s *SerialBuffer) GetBaud() int {
	return s.Baud
}

func (s *SerialBuffer) SetPromptChar(char byte) {
	s.PromptChar = char
}

func (s *SerialBuffer) GetPromptChar() byte {
	return s.PromptChar
}

func (s *SerialBuffer) Init() bool {
	c := &serial.Config{Name: s.SerialPort, Baud: s.Baud}
	srw, err := serial.OpenPort(c)
	if err != nil {
		return false
	}
	s.Serial = srw
	//s.Scanner = bufio.NewScanner(s)
	s.BufPtr = 0
	s.CurPtr = 0
	return true
}

func (s *SerialBuffer) ReadLn() (string, bool) {
	var line string
	if s.PromptChar > 0 {
		s.GotPrompt = false
	}
	if s.CurPtr != s.BufPtr {
		line = s.Lines[s.CurPtr]
		s.CurPtr += 1
		if s.CurPtr >= MAX_LINE_BUF {
			s.CurPtr = 0
		}
		return line, true
	}
	c := make(chan []byte, 1)
	go func() { d, _ :=  s.Read(); c <- d } ()
	select {
		case data := <- c:
			if len(data) > 0 {
				//fmt.Println("DEBUG: data ",data)
			}
		case <- time.After(2 * time.Second):
			fmt.Println("DEBUG: Timeout")
	}
	s.ParseLines()
	// Check again.  If BufPtr is different we got a new line
	if s.CurPtr != s.BufPtr {
		line = s.Lines[s.CurPtr]
		s.CurPtr += 1
		if s.CurPtr >= MAX_LINE_BUF {
			s.CurPtr = 0
		}
		return line, true
	}
	return line, false
}

func (s *SerialBuffer) ParseLines() {
	bufLen := len(s.TmpBuf)
	newLine := make([]byte, 128)
	idx := 0
	newIdx := 0
	lastEOL := 0
	for idx < bufLen {
		if s.TmpBuf[idx] == 13 {
			lastEOL = idx
			if s.StrLen(newLine) > 0 {
				//fmt.Println("DEBUG add newLine", newLine)
				s.AddLine(newLine)
				newLine = make([]byte, 128)
				newIdx = -1
			 }
			newIdx += 1
		} else if s.TmpBuf[idx] == 10 {
			// SKip \n
		} else {
			newLine[newIdx] = s.TmpBuf[idx]
			newIdx += 1
			//fmt.Println("DEBUG: newLine idx", newLine[newIdx])
		}
		idx += 1
	}
	if lastEOL > 0 {
		s.TmpBuf = s.TmpBuf[lastEOL+1:]
	}
	if s.PromptChar > 0 && len(s.TmpBuf) > 0 && s.TmpBuf[len(s.TmpBuf)-1] == s.PromptChar {
		s.GotPrompt = true
	}
}

func (s *SerialBuffer) Read() ([]byte, error) {
        buf := make([]byte, 128)
        n, err := s.Serial.Read(buf)
	//fmt.Println("DEBUG read n bytes: ", n)
        if err != nil {
                return nil, err
        }
	idx := 0
	for idx < n {
		s.TmpBuf = append(s.TmpBuf, buf[idx])
		idx += 1
	}
	//fmt.Println("DEBUG s.TmpBuf=",string(s.TmpBuf), s.TmpBuf)
        return buf[:n], err
}

func (s *SerialBuffer) AddLine(line []byte) {
	s.Lines[s.BufPtr] = string(line)
	s.BufPtr += 1
	if s.BufPtr >= MAX_LINE_BUF {
		s.BufPtr = 0
	}
}

func (s *SerialBuffer) Write(data []byte) error {
	_, err := s.Serial.Write(data)
	return err
}

func (s *SerialBuffer) Writeln(line string) (err error) {
	line = line + "\r"
	err = s.Write([]byte(line))
	return
}

func (s *SerialBuffer) StrLen(data []byte) int {
	l := len(data)
	idx := 0
	for idx < l {
		if data[idx] == 0 {
			return idx
		}
		idx += 1
	}
	return l
}
