package main

import (
	"context"
	"log"
	"net/http"
	"os"
	"os/signal"

	"cdp_xss_test/fetcher"
)

func main() {
	f := fetcher.New("ws://chrome:9222")
	ctx, cancel := context.WithCancel(context.Background())
	sigC := make(chan os.Signal, 1)
	signal.Notify(sigC, os.Interrupt)

	as := NewApp(f)
	srv := http.Server{
		Addr:    ":3333",
		Handler: as.Handler(),
	}

	go func() {
		<-ctx.Done()
		srv.Shutdown(ctx)
	}()

	go func() {
		select {
		case <-sigC:
			cancel()
		}
	}()

	log.Fatalf("Server error: %v", srv.ListenAndServe())
}
