Overview
--------
This is the Go code for the new Canibus Server and sample client

To build set your $GOPATH to here

Install all dependencies you don't have in your go Env (see below)

go install all

./bin/canibusd

You should be able to connect to the web interface on localhost:2515

This is Canibusd version 0.0.2  The API protocol has changed

See the Hive13 wiki for mroe information

Dependencies
-------------
go get github.com/tarm/goserial
go get github.com/gorilla/securecookie
go get github.com/gorilla/sessions
go get github.com/gorilla/mux

Notes
-----
CANiBUS has switch to a Single Page Application (SPA) model recently.
It has a RESTful interface

Routes
------
*  /                     - Homepage
*  /login                - Authentication
*  /lobby                - Lobby
*  /candevices           - JSON list of CAN devices
*  /candevice/:id/config - Configure CAN device
*  /candevice/:id/join   - Join a CAN HackSession 
*  /candevice/:id/info   - JSON CAN Device info
*  /hax/:id              - Sniff session on device
*  /hax/:id/start        - Start the sniffer
*  /hax/:id/stop         - Stop the sniffer
*  /hax/:id/packets      - Pending packets

Original PoC
------------
The Original C++ and NCurses code is now under the foloer orig_poc/

