/*
 * Handles page requests
 */
package webserver

import (
	"io/ioutil"
	"path"
)

var web_root string

type Page struct {
	Body []byte
}

func loadPage(filename string) (*Page, error) {
	body, err := ioutil.ReadFile(path.Join(web_root, filename))
	if err != nil {
		return nil, err
	}
	return &Page{Body: body}, nil
}


