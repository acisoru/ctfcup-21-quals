package urlsafe

import (
	"errors"
	"log"
	"net"
	"net/http"
	"net/url"
	"strings"
	"time"
)

var HttpTimeout = 1 * time.Second

var RedirectsNotAllowedError = errors.New("redirects are not allowed")

func HaveRedirect(purl string) bool {
	client := http.Client{
		Timeout: HttpTimeout,
		CheckRedirect: func(req *http.Request, via []*http.Request) error {
			return RedirectsNotAllowedError
		},
	}
	_, err := client.Get(purl)
	if err != nil && errors.Unwrap(err) == RedirectsNotAllowedError {
		return true
	}
	return false
}

func Check(rawurl string) (*url.URL, bool) {
	u, e := url.Parse(rawurl)
	if e != nil {
		log.Printf("Failed to parse url '%s': %v", rawurl, e)
		return u, false
	}
	if !strings.HasPrefix(u.Scheme, "http") {
		return u, false
	}
	ips, err := net.LookupIP(u.Hostname())
	if err != nil {
		log.Printf("Could not get IPs for url '%v': %v", rawurl, err)
		return u, false
	}
	for _, ip := range ips {
		if !IsPublicIPAddress(ip) {
			log.Printf("%v(%v) is private", u.Hostname(), ip)
			return u, false
		}
	}
	return u, true
}

