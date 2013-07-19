/*
 * Handles web interface
 */
package webserver

import (
	"canibus/logger"
	"canibus/api"
	"fmt"
	"net/http"
	"github.com/gorilla/securecookie"
	"github.com/gorilla/sessions"
)

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
	http.Redirect(w, r, "/lobby", http.StatusFound)
}

func lobbyHandler(w http.ResponseWriter, r *http.Request) {
	auth_err := checkAuth(w, r)
	if auth_err != nil { return }
	p, err := loadPage("lobby.html")
	if err != nil {
		http.Error(w, err.Error(), http.StatusNotFound)
		return
	}
	session, _ := store.Get(r, "canibus")
	fmt.Fprintf(w, "%s: %s", session.Values["user"], p.Body)
}

func configHandler(w http.ResponseWriter, r *http.Request) {

}

func hacksessionHandler(w http.ResponseWriter, r *http.Request) {

}

func StartWebListener(root string, ip string, port string) error {
	web_root = root
	http.HandleFunc("/", rootHandler)
	http.HandleFunc("/login", loginHandler)
	http.HandleFunc("/logout", logoutHandler)
	http.HandleFunc("/lobby", lobbyHandler)
	http.HandleFunc("/config", configHandler)
	http.HandleFunc("/hacksession", hacksessionHandler)
        remote := ip + ":" + port
	logger.Log("Starting CANiBUS Web server on " + remote)
	err := http.ListenAndServe(remote, nil)
	if err != nil {
		return logger.Err("Could not bind web to port: " + err.Error())
	}
	return nil
}

