package main

import (
	"errors"
	"fmt"
	"io"
	"net/http"
	"os"
	"path"
	"strings"

	"github.com/go-rod/rod"
	"github.com/go-rod/rod/lib/launcher"
	"github.com/go-rod/rod/lib/proto"
)

const URL = "https://foursouls.com/card-search/page/%d/?card_type=%s"
const pageCount = 43
const outPath = "result"

var cardTypes = map[string]int{
	"treasure": 6,
	"loot":     3,
	"monster":  7,
	"bsoul":    1,
	"room":     2,
}

func checkErr(err error) {
	if err != nil {
		panic(err)
	}
}

func initBrowser() (*rod.Browser, error) {
	u := launcher.New().
		// Set("--blink-settings=imagesEnabled=false").
		// Headless(false).
		MustLaunch()

	result := rod.New().ControlURL(u)
	err := result.Connect()
	if err != nil {
		return nil, err
	}
	return result, nil
}

func main() {
	browser, err := initBrowser()
	checkErr(err)
	defer browser.Close()
	for ctype, pageCount := range cardTypes {
		scrapeCardType(browser, ctype, pageCount)
	}
}

func scrapeCardType(browser *rod.Browser, cardType string, pageCount int) {
	os.MkdirAll(path.Join(outPath, cardType), os.ModePerm)
	for i := 1; i <= pageCount; i++ {
		scrapePage(browser, fmt.Sprintf(URL, i, cardType), cardType)
	}
}

func scrapePage(browser *rod.Browser, url, cardType string) {
	page, err := browser.Page(proto.TargetCreateTarget{})
	checkErr(err)
	defer page.Close()

	err = page.Navigate(url)
	checkErr(err)

	err = page.WaitLoad()
	checkErr(err)

	els, err := page.Elements(".cardGridCell")
	checkErr(err)
	for _, el := range els {
		image, err := el.Element("img")
		checkErr(err)
		link, err := image.Attribute("src")
		checkErr(err)
		imPath := *link
		if !strings.HasSuffix(imPath, ".png") {
			link, err = image.Attribute("data-src")
			checkErr(err)
			imPath = *link
		}
		nameP, err := image.Attribute("alt")
		checkErr(err)
		name := strings.ReplaceAll(*nameP, "/", "_")

		fmt.Println(*link)
		imPath = name + ".png"
		op := path.Join(outPath, cardType, imPath)
		if _, err := os.Stat(op); errors.Is(err, os.ErrNotExist) {
			err = downloadFile(*link, op)
			checkErr(err)
			fmt.Printf("%s downloaded!\n", imPath)
			continue
		}
		fmt.Printf("%s already exists\n", op)
	}
}

// func scrapePage(browser *rod.Browser, n int) {
// 	url := fmt.Sprintf("%s%d", URL+"index.php?page=", n)
// 	fmt.Printf("Connecting to page %v...\n", url)
// 	page, err := browser.Page(proto.TargetCreateTarget{})
// 	checkErr(err)
// 	defer page.Close()

// 	err = page.Navigate(url)
// 	checkErr(err)

// 	err = page.WaitLoad()
// 	checkErr(err)

// 	els, err := page.Elements(imageSelector)
// 	checkErr(err)
// 	if len(els) == 0 {
// 		colorwrapper.Printf("red", "Failed to fetch cards from page %d\n", n)
// 		return
// 	}
// 	for _, el := range els {
// 		im, err := el.Element("img")
// 		checkErr(err)
// 		src, err := im.Attribute("src")
// 		checkErr(err)
// 		name := clearName(*src)
// 		err = downloadFile(URL+*src, path.Join(outPath, name))
// 		checkErr(err)
// 		colorwrapper.Printf("cyan", "\tCard %s downloaded\n", name)
// 		// fmt.Println()
// 	}
// }

func downloadFile(imURL, fileName string) error {
	//Get the response bytes from the url
	response, err := http.Get(imURL)
	if err != nil {
		return err
	}
	defer response.Body.Close()

	if response.StatusCode != 200 {
		return errors.New("Received non 200 response code")
	}
	//Create a empty file
	file, err := os.Create(fileName)
	if err != nil {
		return err
	}
	defer file.Close()

	//Write the bytes to the fiel
	_, err = io.Copy(file, response.Body)
	if err != nil {
		return err
	}

	return nil
}

// func clearName(name string) string {
// 	return strings.TrimPrefix(name, "data/cards/")
// }
