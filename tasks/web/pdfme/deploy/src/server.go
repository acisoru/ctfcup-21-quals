package main

import (
	"context"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"time"

	"github.com/labstack/echo/v4"

	"cdp_xss_test/fetcher"
	"cdp_xss_test/urlsafe"
)

func NewApp(fetch *fetcher.Fetcher) *AppServer {
	as := &AppServer{
		srv:   echo.New(),
		fetch: fetch,
	}
	as.initRoutes()
	return as

}

type AppServer struct {
	srv   *echo.Echo
	fetch *fetcher.Fetcher
}

func (as *AppServer) Handler() http.Handler {
	return as.srv
}

func (as *AppServer) errorJSON(err string) map[string]string {
	x := make(map[string]string)
	x["error"] = err
	return x
}

func (as *AppServer) initRoutes() {
	as.srv.POST("/pdf", as.handleFetch)
	as.srv.File("/", "static/index.html")
}

func (as *AppServer) handleFetch(ctx echo.Context) error {
	url := ctx.FormValue("url")
	parsed, ok := urlsafe.Check(url)
	if !ok {
		return ctx.JSON(http.StatusPreconditionFailed, as.errorJSON("URL is not supported"))
	}
	purl := parsed.String()
	if urlsafe.HaveRedirect(purl) {
		log.Printf("Redirect found on URL '%v'(%v)", url, purl)
		return ctx.JSON(http.StatusPreconditionFailed, as.errorJSON("Redirects are not allowed"))
	}
	exctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	data, err := as.fetch.PDFUrl(exctx, purl)
	if err != nil {
		return ctx.JSON(http.StatusPreconditionFailed, as.errorJSON(err.Error()))
	}
	f, err := ioutil.TempFile("", "")
	if err != nil {
		return ctx.JSON(http.StatusPreconditionFailed, as.errorJSON(err.Error()))
	}
	defer os.Remove(f.Name())
	if _, err := f.Write(data); err != nil {
		return ctx.JSON(http.StatusPreconditionFailed, as.errorJSON(err.Error()))
	}
	return ctx.Attachment(f.Name(), parsed.Host+".pdf")
}
