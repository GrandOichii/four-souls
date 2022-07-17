package main

import (
	"encoding/json"
	"os"
	"strconv"

	"github.com/GrandOichii/colorwrapper"
	"github.com/go-rod/rod"
	"github.com/go-rod/rod/lib/launcher"
	"github.com/go-rod/rod/lib/proto"
)

type Card struct {
	Name string `json:"name"`
	Text string `json:"text"`

	Health int `json:"health"`
	Roll   int `json:"roll"`
	Power  int `json:"power"`
}

// https://foursouls.com/card-search/?card_type=loot
const URL = "https://foursouls.com"
const cardSearchURL = URL + "/card-search/?card_type="

const resulPath = "result-monsters.json"

var result = map[string][]Card{}

var cardTypes = []string{
	// "loot",
	// "character",
	// "eternal",
	// "treasure",
	"monster",
}

func checkErr(err error) {
	if err != nil {
		data, err := json.MarshalIndent(result, "", "\t")
		checkErr(err)
		err = os.WriteFile(resulPath, data, 0755)
		checkErr(err)

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

	for _, t := range cardTypes {
		scrapeCards(browser, t)
	}

	data, err := json.MarshalIndent(result, "", "\t")
	checkErr(err)
	err = os.WriteFile(resulPath, data, 0755)
	checkErr(err)
}

func scrapeCards(browser *rod.Browser, cardType string) {
	url := cardSearchURL + cardType
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
		a, err := el.Element("a")
		checkErr(err)

		href, err := a.Attribute("href")
		checkErr(err)

		card := scrapePage(browser, *href)
		result[cardType] = append(result[cardType], card)
	}
}

func scrapePage(browser *rod.Browser, href string) Card {
	page, err := browser.Page(proto.TargetCreateTarget{})
	checkErr(err)
	defer page.Close()

	err = page.Navigate(href)
	checkErr(err)

	err = page.WaitLoad()
	checkErr(err)

	// for monsters
	el, err := page.Element(".cardpage")
	checkErr(err)

	nameEl, err := el.Element("h1")
	checkErr(err)
	name, err := nameEl.Text()
	checkErr(err)

	el, err = page.Element(".StatTable")
	checkErr(err)

	el, err = el.Element("tbody")
	checkErr(err)

	stat, err := el.Element("tr")
	checkErr(err)
	health, err := strconv.Atoi(stat.MustElement(".value").MustText())

	stat, err = stat.Next()
	checkErr(err)
	roll, err := strconv.Atoi(stat.MustElement(".value").MustText())

	stat, err = stat.Next()
	checkErr(err)
	power, err := strconv.Atoi(stat.MustElement(".value").MustText())
	// textEl, err := el.Element(".effectOutcome")
	// checkErr(err)
	// text, err := textEl.Text()
	// checkErr(err)

	result := Card{}
	result.Name = name
	result.Health = health
	result.Roll = roll
	result.Power = power
	colorwrapper.Printf("cyan", "Card %s scraped\n", result.Name)
	return result
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

// func downloadFile(imURL, fileName string) error {
// 	//Get the response bytes from the url
// 	response, err := http.Get(imURL)
// 	if err != nil {
// 		return err
// 	}
// 	defer response.Body.Close()

// 	if response.StatusCode != 200 {
// 		return errors.New("Received non 200 response code")
// 	}
// 	//Create a empty file
// 	file, err := os.Create(fileName)
// 	if err != nil {
// 		return err
// 	}
// 	defer file.Close()

// 	//Write the bytes to the fiel
// 	_, err = io.Copy(file, response.Body)
// 	if err != nil {
// 		return err
// 	}

// 	return nil
// }

// func clearName(name string) string {
// 	return strings.TrimPrefix(name, "data/cards/")
// }
