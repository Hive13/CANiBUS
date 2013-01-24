// Package Logger provides a generic logging function
package logger

import (
	"fmt"
	"time"
)

type LogMsg struct {
	When time.Time
	What string
}

// toString Converts a log message to a sting.  Needs to be initialized first
func (l *LogMsg) toString() string {
	return fmt.Sprintf("[%v] %s", l.When, l.What)
}

// Log is the main logging method
func Log(msg string) {
	l := LogMsg{time.Now(), msg}
	fmt.Println(l.toString())
}

// err sets logmsg as an error string and returns an error interface
func Err(msg string) error {
	return &LogMsg{time.Now(), msg}
}

// Error returns a string with date/time like toString
func (l *LogMsg) Error() string {
	return l.toString()
}
