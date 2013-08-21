#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include "serial.h"

Serial::Serial()
{
	m_fd = -1;
	m_port = "/dev/ttyS0";
	m_baudrate = 38400;
}

Serial::~Serial()
{
	if(m_fd > 0)
		::close(m_fd);
}

int Serial::open()
{
	struct serial_struct tty;
	struct termios options;
        int flags, modemflags;
	m_fd = ::open(m_port, O_RDWR | O_NOCTTY | O_NDELAY);
	if(m_fd < 1) {
		fprintf(stderr, "Could not open serial %s\n", m_port);
		return m_fd;
	}

	if(ioctl(m_fd, TIOCGSERIAL, &tty) < 0) {
		fprintf(stderr, "TIOCGSERIAL failed\n");
		return -1;
	}

	if(ioctl(m_fd, TIOCMGET, &modemflags) < 0) {
		fprintf(stderr, "Could not get modem info\n");
		return -1;
	}

	bzero(&options, sizeof(options));
	if(tcgetattr(m_fd, &options) < 0) {
		fprintf(stderr, "Could not run tcgetattr\n");
		return -1;
	}
	
	// Enable polling, could be moved to a seperate method
        //flags = fcntl(m_fd, F_GETFL, 0);
	// May also need FNDELAY
        //fcntl(m_fd, F_SETFL, flags | O_NONBLOCK | FNDELAY);

	switch(m_baudrate)
	{
	case 9600:
		cfsetispeed(&options, B9600);
		cfsetospeed(&options, B9600);
		break;
	case 38400: 
		if (cfsetispeed(&options, B38400) < 0) {
			fprintf(stderr, "cfsetispeed failed\n");
			return -1;
		}
		if (cfsetospeed(&options, B38400) < 0) {
			fprintf(stderr, "cfsetospeed failed\n");
			return -1;
		}
		break;
	case 57600:
		cfsetispeed(&options, B57600);
		cfsetospeed(&options, B57600);
		break;
	case 115200:
		cfsetispeed(&options, B115200);
		cfsetospeed(&options, B115200);
		break;
	default:
		cfsetispeed(&options, B38400);
		cfsetospeed(&options, B38400);
	}
	/* Not going to mess with ioctl at this time
		tty.custom_divisor = tty.baud_base / B38400;
		tty.flags &= ~ASYNC_SPD_MASK;
		tty.flags |= ASYNC_SPD_CUST | ASYNC_LOW_LATENCY;
		if(ioctl(m_fd, TIOCSSERIAL, &tty) < 0) {
			fprintf(stderr, "ioctl TIOCSSERIAL failed\n");
			return -1;
		}
	*/

	// disable HW/SW Flow control
	options.c_cflag &= ~CRTSCTS;
	options.c_iflag &= ~(IXON | IXOFF| IXANY);

	// 8n1
    	options.c_cflag |= CS8;
	options.c_cflag &= ~PARENB;

	options.c_cc[VMIN]  = 0;
    	options.c_cc[VTIME] = 20;

	tcflush(m_fd, TCIFLUSH);
	tcsetattr(m_fd, TCSANOW, &options);

	return m_fd;
}

void Serial::close()
{
	if(m_fd > 0)
		::close(m_fd);
}

void Serial::ioWrite(std::string data)
{
	int n = write(m_fd, data.c_str(), data.length());
}

std::string Serial::readLine()
{
        std::string line;
        int n;
        char buf[255];

        n = read(m_fd, buf, 255);
        if(n > 0) {
                // read up to \n
                for(int i=0; i < n; i++) {
                        if(buf[i] == '\r') {
                                m_lineQ.push(m_line);
                                m_line.clear();
                        } else {
                                if(buf[i] != 0 && buf[i] != '\n')
                                        m_line += buf[i];
                        }
		}
        }
        if(m_lineQ.size() > 0) {
                line = m_lineQ.front();
		// Debugging
		//fprintf(stderr, "SERIAL LINE: %s\n", line.c_str());
                m_lineQ.pop();
        }
        return line;
}

