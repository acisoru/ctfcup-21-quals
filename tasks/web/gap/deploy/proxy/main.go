package main

import (
	//"crypto/tls"
	"fmt"
	"html/template"
	"io/ioutil"
	"log"
	"strings"
	"net/http"
)

type Page struct {
	Title string
	URL template.URL
	Body  template.HTML
}

func loadPage() (*Page, error) {
	_, _ = ioutil.ReadFile("index.html")
	return &Page{Title: "My simple proxy server", URL: "", Body: "body"}, nil
}

func handler(w http.ResponseWriter, r *http.Request) {
	t, _ := template.ParseFiles("index.html")
	t.Execute(w, &Page{Title: "My simple proxy"})
	//
	//f, err := s.Fs.Open("index.html")
	//if err != nil {
	//	http.Error(w, r.RequestURI, StatusNotFound)
	//	return
	//}
	//defer f.Close()
	//
	//fmt.Fprintf(w, "Hi there, I love %s!", r.URL.Path[1:])
}

func handlerURL(w http.ResponseWriter, r*http.Request) {
	t, _ := template.ParseFiles("go.html")

	url := r.FormValue("url")
	log.Println(url)
	//tr := &http.Transport{
	//	TLSClientConfig: &tls.Config{InsecureSkipVerify: true},
	//}

	//client := &http.Client{Transport: tr}
	client := &http.Client{}

	s := "https://" + url + "/?test=a"
	s = strings.Replace(s, "\\r", "\r", -1)
	s = strings.Replace(s, "\\n", "\n", -1)
	//s := "http://206.189.18.216:8080?a=1 HTTP/1.1\r\nX-injected: header\r\nTEST: 123/?test=a"
	request, err := http.NewRequest("GET", s, nil)
	if err != nil {
		fmt.Println(err)
		fmt.Printf("request error\n")
	}
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
func main() {
	http.HandleFunc("/", handler)
	http.HandleFunc("/go", handlerURL)
	log.Fatal(http.ListenAndServe(":8000", nil))
}

