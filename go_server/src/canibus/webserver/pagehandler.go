/*
 * Handles page requests
 */
package webserver

import (
	"html/template"
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

func loadTemplate(filename string) (*template.Template, error) {
	t, err := template.ParseFiles(path.Join(web_root, filename))
	if err != nil {
		return nil, err
	}
	return t, nil
}
