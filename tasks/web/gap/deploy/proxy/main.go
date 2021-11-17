package main

import (
	"crypto/tls"
	"fmt"
	"html/template"
	"io/ioutil"
	"log"
	"net/http"
	"strings"
)

type Page struct {
	Title string
	URL   template.URL
	Body  template.HTML
}

func handler(w http.ResponseWriter, r *http.Request) {
	t, _ := template.ParseFiles("index.html")
	w.Header().Set("Server", "go httpserver 1.11.5")
	t.Execute(w, &Page{Title: "My simple proxy"})
}

func handlerURL(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Server", "go httpserver 1.11.5")

	t, _ := template.ParseFiles("go.html")

	url := r.FormValue("url")
	log.Println(url)
	tr := &http.Transport{
		TLSClientConfig: &tls.Config{InsecureSkipVerify: true},
	}

	client := &http.Client{Transport: tr}

	s := "https://" + url + ""
	s = strings.Replace(s, "\\r", "\r", -1)
	s = strings.Replace(s, "\\n", "\n", -1)

	request, err := http.NewRequest("GET", s, nil)
	if err != nil {
		fmt.Println(err)
		fmt.Printf("request error\n")
	} else {
		resp, err := client.Do(request)
		if err != nil {
			fmt.Println(err)
			fmt.Printf("response error\n")
			//body, err
		} else {
			defer resp.Body.Close()
			body, _ := ioutil.ReadAll(resp.Body)
			t.Execute(w, &Page{Title: "My simple proxy", URL: template.URL(url), Body: template.HTML(body)})
		}
	}

}
func main() {
	http.HandleFunc("/", handler)
	http.HandleFunc("/go", handlerURL)
	log.Fatal(http.ListenAndServe(":8000", nil))
}
