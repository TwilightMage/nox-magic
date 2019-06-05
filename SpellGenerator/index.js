const axios = require('axios')
const {Translate} = require('@google-cloud/translate')

const fs = require('fs')

const yandexapikey = 'trnsl.1.1.20190604T182104Z.3bfabf359d343fdf.c487b2101e9437dc7d4848de67c10a5df42a279d'
const letters = 'אבגדהוזחטיכלמוסעפצקרשת'
const count = 100
const method = -1

function letter() {
	return letters[Math.floor(Math.random() * letters.length)]
}

function word() {
	return letter() + letter() + letter()
}


function failed(index)
{
	console.log(index, 'fail')
	fs.appendFileSync('output.txt', `${index} fail\r\n`)
}

function translatedNoCheck(index, text) {
	console.log(index, text)
	fs.appendFileSync('output.txt', `${index} ${text}\r\n`)
}

function errored(index)
{
	console.log(index, 'error')
	fs.appendFileSync('output.txt', `${index} error\r\n`)
}

function translated(index, text)
{
	let success = true
	for (let i = 0; i < letters.length; i++) {
		if (text.includes(letters[i])) {
			success = false
			break
		}
	}
	
	if (success) translatedNoCheck(index, text)
	else failed(index)
}

(async () => {
	if (fs.existsSync('output.txt')) fs.unlinkSync('output.txt')
	switch (method) {
		case -1:
			console.log('used method -1 - no translation')
			for (let i = 0; i < count; i++) translatedNoCheck(i, word())
			break

		case 0:
			console.log('used method 0 - yandex translate')
			for (let i = 0; i < count; i++) {
				axios.get('https://translate.yandex.net/api/v1.5/tr.json/translate', {
					params: {
						key: yandexapikey,
						text: word(),
						lang: 'he-en'
					}
				}).then((response) => {
					translated(i, response.data.text)
				}).catch((error) => {
				    errored(i)
			  	})	
			}
		  	break
		  
	  	case 1:
	  		console.log('used method 1 - google translate')
	  		const translate = new Translate()

	  		let words = []
	  		for (let i = 0; i < count; i++) words.push(word())

	  		try {
				let translations = await translate.translate(words, 'en');
				translations = Array.isArray(translations) ? translations : [translations];
				translations.forEach((translation, i) => {
				  translated(i, translation)
				})
	  		} catch (exc) {
	  			for (let i = 0; i < count; i++) errored(i)
	  		}
	  		break

	  	default:
	  		console.log(`used undefined method ${method}`)
	  		break
	}
})()