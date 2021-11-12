package main

import (
	"context"
	"errors"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"os/exec"
	"strings"
	"time"
)

const PHP_DIR_PATH = "/app/php_scripts"

func execScript(script string, param string) (string, error) {
	if !strings.HasSuffix(script, ".php") {
		return "", errors.New("only php scripts are allowed")
	}
	ctx, cancel := context.WithTimeout(context.Background(), time.Second*1)
	defer cancel()
	cmd := exec.CommandContext(ctx, "php", script, param)
	cmd.Dir = PHP_DIR_PATH
	res, err := cmd.Output()
	return string(res), err
}

func showSource(w http.ResponseWriter, r *http.Request) {
	f, err := os.Open("/app/main.go")
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Err = %v", err)
		return
	}
	_, err = io.Copy(w, f)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Err = %v", err)
	}
	return
}

func runScript(w http.ResponseWriter, r *http.Request) {
	script, param := r.FormValue("script"), r.FormValue("param")
	res, err := execScript(script, param)
	if err != nil {
		w.WriteHeader(http.StatusInternalServerError)
		fmt.Fprintf(w, "Err run script %v (with param %v)= %v", script, param, err)
		return
	}
	fmt.Fprintf(w, "<h3>The result is \"%v\"</h3>", res)
	return
}

func logRequest(handler http.Handler) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		log.Printf("%s %s %s\n", r.RemoteAddr, r.Method, r.URL)
		handler.ServeHTTP(w, r)
	})
}

func main() {
	http.HandleFunc("/", showSource)
	http.HandleFunc("/run", runScript)
	fs := http.FileServer(http.Dir(PHP_DIR_PATH))

	http.Handle("/phpscripts/", http.StripPrefix("/phpscripts", fs))
	if err := http.ListenAndServe(":3000", logRequest(http.DefaultServeMux)); err != nil {
		log.Fatal(err)
	}

}
