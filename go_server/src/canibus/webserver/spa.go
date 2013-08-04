package webserver

import (
	"canibus/core"
	"canibus/logger"
	"github.com/gorilla/mux"
	"fmt"
	"net/http"
	"encoding/json"
)

type CanDeviceJSON struct {
	Id int
	DeviceType string
	DeviceDesc string
	HackSession string
}

func homeHandler(w http.ResponseWriter, r *http.Request) {
        p, err := loadPage("index-spa.html")
        if err != nil {
                http.Error(w, err.Error(), http.StatusNotFound)
                return
        }
        fmt.Fprintf(w, "%s", p.Body)
}

func partialLobbyHandler(w http.ResponseWriter, r *http.Request) {
	logger.Log("Lobby checking auth...")
	auth_err := checkAuth(w, r)
	if auth_err != nil { return }
        p, err := loadPage("partials/lobby.html")
        if err != nil {
                http.Error(w, err.Error(), http.StatusNotFound)
                return
        }
        fmt.Fprintf(w, "%s", p.Body)
}

func configCanHandler(w http.ResponseWriter, r *http.Request) {
	logger.Log("Config CAN Device, checking auth...")
	auth_err := checkAuth(w, r)
	if auth_err != nil { return }
	//vars := mux.Vars(r)
	//canId := vars["id"]
        p, err := loadPage("partials/config.html")
        if err != nil {
                http.Error(w, err.Error(), http.StatusNotFound)
                return
        }
        fmt.Fprintf(w, "%s", p.Body)
}

func joinHaxHandler(w http.ResponseWriter, r *http.Request) {
	//vars := mux.Vars(r)
	//canId := vars["id"]
	// TODO: Check HackSession state and either config or sniff
        p, err := loadPage("partials/config.html")
        if err != nil {
                http.Error(w, err.Error(), http.StatusNotFound)
                return
        }
        fmt.Fprintf(w, "%s", p.Body)
}


func candevicesHandler(w http.ResponseWriter, r *http.Request) {
	config := core.GetConfig()
	drivers := config.GetDrivers()
	var data []CanDeviceJSON
	for i := range drivers {
		dev := CanDeviceJSON{}
		dev.Id = drivers[i].GetId()
		dev.DeviceType = drivers[i].DeviceType()
		dev.DeviceDesc = drivers[i].DeviceDesc()
		hax := drivers[i].GetHackSession()
		if hax == nil {
			dev.HackSession = "Idle"
		} else {
			dev.HackSession = hax.GetState()
		}
		data = append(data, dev)
	}
	j, err := json.Marshal(data)
	if err != nil {
		logger.Log("Could not convert candevices to json")
		return
	}
	fmt.Fprintf(w, "%s", j)
}

func StartSPAWebListener(root string, ip string, port string) error {
	web_root = root
	r := mux.NewRouter()
	r.HandleFunc("/", homeHandler)
	r.HandleFunc("/login", loginHandler)
	r.HandleFunc("/logout", logoutHandler)
	r.HandleFunc("/partials/lobby.html", partialLobbyHandler)
	r.HandleFunc("/candevice/{id}/config", configCanHandler)
	r.HandleFunc("/candevice/{id}/join", joinHaxHandler)
	r.HandleFunc("/candevices", candevicesHandler)

	http.Handle("/partials/",http.FileServer(http.Dir(root)))
	http.Handle("/js/",http.FileServer(http.Dir(root)))
	http.Handle("/css/", http.FileServer(http.Dir(root)))
	http.Handle("/fonts/", http.FileServer(http.Dir(root)))
	http.Handle("/images/", http.FileServer(http.Dir(root)))
	http.Handle("/bootstrap/", http.FileServer(http.Dir(root)))
	http.Handle("/", r)
        remote := ip + ":" + port
        logger.Log("Starting CANiBUS Web server on " + remote)
        err := http.ListenAndServe(remote, nil)
        if err != nil {
                return logger.Err("Could not bind web to port: " + err.Error())
        }
        return nil
}
