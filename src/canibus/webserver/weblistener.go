/*
 * Handles web interface
 */
package webserver

import (
	"canibus/api"
	"canibus/canibususer"
	"canibus/core"
	"canibus/hacksession"
	"canibus/logger"
	"fmt"
	"github.com/gorilla/securecookie"
	"github.com/gorilla/sessions"
	"net/http"
	"strconv"
	//"code.google.com/p/go.net/websocket"
)

type LobbyTemplate struct {
	Host       string
	Config     api.Configer
	NumOfUsers int
}

type ConfigTemplate struct {
	Device api.CanDevice
}

type connection struct {
	// The websocket connection.
	//ws *websocket.Conn

	// Buffered channel of outbound messages.
	send chan string
}

type hub struct {
	// Registered connections.
	connections map[*connection]bool

	// Inbound messages from the connections.
	broadcast chan string

	// Register requests from the connections.
	register chan *connection

	// Unregister requests from connections.
	unregister chan *connection
}

var h = hub{
	broadcast:   make(chan string),
	register:    make(chan *connection),
	unregister:  make(chan *connection),
	connections: make(map[*connection]bool),
}

var store = sessions.NewCookieStore(securecookie.GenerateRandomKey(32))

func checkAuth(w http.ResponseWriter, r *http.Request) error {
	session, _ := store.Get(r, "canibus")
	if session.Values["user"] == nil {
		http.Redirect(w, r, "/", http.StatusFound)
		return logger.Err("Not authenticated")
	}
	return nil
}

func logoutHandler(w http.ResponseWriter, r *http.Request) {
	session, _ := store.Get(r, "canibus")
	session.Values["user"] = nil
	session.Save(r, w)
	http.Redirect(w, r, "/", http.StatusFound)
}

func rootHandler(w http.ResponseWriter, r *http.Request) {
	p, err := loadPage("index.html")
	if err != nil {
		http.Error(w, err.Error(), http.StatusNotFound)
		return
	}
	fmt.Fprintf(w, "%s", p.Body)
}

func loginHandler(w http.ResponseWriter, r *http.Request) {
	user := r.FormValue("username")
	logger.Log("User logged in: " + user)
	cmd := &api.Cmd{}
	cmd.Action = "Login"
	cmd.Arg = make([]string, 1)
	cmd.Arg[0] = user
	err := api.ProcessLogin(cmd)
	if err != nil {
		http.Redirect(w, r, "/", http.StatusFound)
		return
	}
	session, _ := store.Get(r, "canibus")
	session.Values["user"] = user
	session.Save(r, w)
	logger.Log("Cookie saved")
	var NewUser canibususer.CanibusUser
	NewUser.SetName(user)
	core.AddUser(&NewUser)
	fmt.Fprintf(w, "OK")
	//http.Redirect(w, r, "/#/lobby", http.StatusFound)
}

func lobbyHandler(w http.ResponseWriter, r *http.Request) {
	auth_err := checkAuth(w, r)
	if auth_err != nil {
		return
	}
	t, err := loadTemplate("lobby.html")
	if err != nil {
		http.Error(w, err.Error(), http.StatusNotFound)
		return
	}
	session, _ := store.Get(r, "canibus")
	userName := session.Values["user"].(string)
	user, _ := core.GetUserByName(userName)
	if user.GetDeviceId() != 0 {
		leaveDevice(user)
	}
	data := LobbyTemplate{}
	data.Host = r.Host
	data.Config = core.GetConfig()
	data.NumOfUsers = core.NumberOfUsers()
	exec_err := t.Execute(w, data)
	if exec_err != nil {
		fmt.Println("Lobby Error: ", exec_err)
	}
}

func configHandler(w http.ResponseWriter, r *http.Request) {
	auth_err := checkAuth(w, r)
	if auth_err != nil {
		return
	}
	t, err := loadTemplate("config.html")
	if err != nil {
		http.Error(w, err.Error(), http.StatusNotFound)
		return
	}
	str_id := r.FormValue("id")
	id, id_err := strconv.Atoi(str_id)
	if id_err != nil {
		http.Error(w, err.Error(), http.StatusNotFound)
		return
	}
	dev, dev_err := core.GetDeviceById(id)
	if dev_err != nil {
		http.Error(w, dev_err.Error(), http.StatusNotFound)
		return
	}
	session, _ := store.Get(r, "canibus")
	userName := session.Values["user"].(string)
	user, _ := core.GetUserByName(userName)
	// Now that we know we have a device setup hack session
	if dev.GetHackSession() == nil {
		hacks := hacksession.HackSession{}
		hacks.SetState(hacksession.STATE_CONFIG)
		hacks.SetDeviceId(dev.GetId())
		user.SetDeviceId(dev.GetId())
		dev.SetHackSession(&hacks)
	}
	data := ConfigTemplate{}
	data.Device = dev
	exec_err := t.Execute(w, data)
	if exec_err != nil {
		fmt.Println("Config Error: ", exec_err)
	}
}

func hacksessionHandler(w http.ResponseWriter, r *http.Request) {

}

// Cleans up user module when exiting a device
func leaveDevice(user api.User) {
	dev_id := user.GetDeviceId()
	if dev_id == 0 {
		return
	}
	dev, _ := core.GetDeviceById(dev_id)
	hax := dev.GetHackSession()
	if hax == nil {
		return
	}
	hax.RemoveUser(user)
	if hax.NumOfUsers() == 0 {
		dev.SetHackSession(nil)
	}
}

func (h *hub) run() {
	for {
		select {
		case c := <-h.register:
			h.connections[c] = true
		case c := <-h.unregister:
			delete(h.connections, c)
			close(c.send)
		case m := <-h.broadcast:
			for c := range h.connections {
				select {
				case c.send <- m:
				default:
					delete(h.connections, c)
					close(c.send)
					//go c.ws.Close()
				}
			}
		}
	}
}

/*
func (c *connection) reader() {
	for {
		var message string
		err := websocket.Message.Receive(c.ws, &message)
		if err != nil {
			break
		}
		logger.Log("Recieved: " + message)
		h.broadcast <- message
	}
	c.ws.Close()
}

func (c *connection) writer() {
	for message := range c.send {
		err := websocket.Message.Send(c.ws, message)
		if err != nil {
			break
		}
	}
	c.ws.Close()
}

func chatLobbyHandler(ws *websocket.Conn) {
	logger.Log("chatLobbyHandler()")
	auth_err := checkAuth(nil, ws.Request())
	if auth_err != nil { return }
        c := &connection{send: make(chan string, 256), ws: ws}
        h.register <- c
        defer func() { h.unregister <- c }()
        go c.writer()
        c.reader()
	logger.Log("done with chatLobbyHandler")
}
*/

func StartWebListener(root string, ip string, port string) error {
	web_root = root
	http.HandleFunc("/", rootHandler)
	http.HandleFunc("/login", loginHandler)
	http.HandleFunc("/logout", logoutHandler)
	http.HandleFunc("/lobby", lobbyHandler)
	http.HandleFunc("/config", configHandler)
	http.HandleFunc("/hacksession", hacksessionHandler)
	//http.Handle("/chatLobby", websocket.Handler(chatLobbyHandler))
	remote := ip + ":" + port
	logger.Log("Starting CANiBUS Web server on " + remote)
	err := http.ListenAndServe(remote, nil)
	if err != nil {
		return logger.Err("Could not bind web to port: " + err.Error())
	}
	return nil
}
