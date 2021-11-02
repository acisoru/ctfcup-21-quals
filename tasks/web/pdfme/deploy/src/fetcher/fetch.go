package fetcher

import (
	"context"
	"fmt"
	"time"

	"github.com/chromedp/cdproto/network"
	"github.com/chromedp/cdproto/page"
	"github.com/chromedp/chromedp"

	"cdp_xss_test/urlsafe"
)

func New(chromePath string) *Fetcher {
	return &Fetcher{
		ChromePath: chromePath,
	}
}

type Fetcher struct {
	ChromePath     string
}


// print a specific pdf page.
func printToPDF(urlstr string, res *[]byte) chromedp.Tasks {
	return chromedp.Tasks{
		chromedp.Navigate(urlstr),
		chromedp.Sleep(50 * time.Millisecond),
		chromedp.ActionFunc(func(ctx context.Context) error {
			buf, _, err := page.PrintToPDF().WithPrintBackground(false).Do(ctx)
			if err != nil {
				return err
			}
			*res = buf
			return nil
		}),
	}
}

func disableRedirects(ctx context.Context) <-chan error {
	out := make(chan error, 1)
	wrapctx, cancel := context.WithCancel(ctx)
	chromedp.ListenTarget(wrapctx, func(ev interface{}) {
		switch ev.(type) {
		case *network.EventRequestWillBeSent:
			requestEv := ev.(*network.EventRequestWillBeSent)
			if requestEv.RedirectResponse != nil {
				out <- urlsafe.RedirectsNotAllowedError
				close(out)
				cancel()
			}
		}
	})
	return out
}

func disableURLs(urls ...string) chromedp.Action {
	return network.SetBlockedURLS(urls)
}

func (f *Fetcher) PDFUrl(ctx context.Context, purl string) ([]byte, error) {
	wsURL := fmt.Sprintf(f.ChromePath)
	allocatorContext, cancel := chromedp.NewRemoteAllocator(ctx, wsURL)
	defer cancel()
	ctxt, cancel := chromedp.NewContext(allocatorContext)
	defer cancel()
	redChan := disableRedirects(ctxt)
	var buf []byte
	var redError error

	go func() {
		select {
		case <-ctxt.Done():
			return
		case redError = <-redChan:
			cancel()
		}
	}()

	err := chromedp.Run(ctxt, disableURLs("*127.0*", "*localhost*"), printToPDF(purl, &buf))
	if redError != nil {
		err = redError
	}
	return buf, err

}
