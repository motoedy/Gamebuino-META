/*
This file is part of the Gamebuino-Meta library,
Copyright (c) Aadalie 2014, 2016-2017

This is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License (LGPL)
as published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

This is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License (LGPL) for more details.

You should have received a copy of the GNU Lesser General Public
License (LGPL) along with the library.
If not, see <http://www.gnu.org/licenses/>.

Authors:
 - Aurelien Rodot
 - Sorunome
*/

#include "Gamebuino-Meta.h"
#include "utility/Graphics-SD.h"
#include "utility/Misc.h"
#include "utility/Language/SystemLanguage.h"	
SdFat SD;

// a 3x5 font table
extern const uint8_t font3x5[];

namespace Gamebuino_Meta {
	
// we have more blocks for, in case we add things in the future, old games are less likely to erase our new blocks
const uint8_t SETTINGSCONF_NUM_BLOCKS = 32;
const SaveDefault settingsDefaults [] = {
	{ SETTING_VOLUME, SAVETYPE_INT, 6, 0 },
	{ SETTING_VOLUME_MUTE, SAVETYPE_INT, 0, 0 },
	{ SETTING_DEFAULTNAME, SAVETYPE_BLOB, {.ptr="gamebuinian"}, 13 },
	{ SETTING_LANGUAGE, SAVETYPE_INT, (int)LangCode::en, 0 },
	{ SETTING_NEOPIXELS_INTENSITY, SAVETYPE_INT, 4, 0 },
};
// neoPixel intensity is quadratic
// 0 is min, 255 is max
// m*x^2 fit through (0,0) (4,255) (5-steps)
// m=15.9375
const uint8_t neoPixelsIntensities[] = {
	0, 16, 64, 143, 255
};

const uint16_t startLightsData[] = {
	2, 4,
	24,
	1,
	0,
	0,
	
	0xc000,0x4000,0x4000,0x1000,0x1000,0x0,0x0,0x0,0x3800,0xa001,0x1000,0x3800,0x0,0x1000,0x0,0x0,0x1000,0x3001,0x0,0x9006,0x0,0x3003,0x0,0x1001,0x0,0x1001,0x0,0x3806,0x0,0xa815,0x1002,0x4008,0x0,0x1003,0x0,0x1003,0x1804,0x5810,0x5814,0xa03f,0x0,0x0,0x1809,0x0,0x501f,0x180d,0x70ff,0x403f,0x1818,0x0,0x305f,0x0,0x69ff,0x0,0x289f,0x101f,0x28bf,0x83f,0x7b5f,0x0,0x20ff,0x0,0x85f,0x0,0x94bf,0x295f,0x299f,0x87f,0x7f,0x0,0x0,0x0,0x221f,0xadff,0xdf,0x1a5f,0x0,0xfe,0x0,0x0,0x153,0xb7f,0x0,0xcf5f,0x0,0x49f,0x0,0x18b,0x0,0x1a9,0x0,0x63d,0x0,0xdfff,0x1c7,0x659,0x0,0x1c6,0x0,0x1c5,0x1c5,0x632,0x62f,0xa7fd,0x0,0x0,0x1a3,0x0,0x5e8,0x1a2,0x6ff7,0x5c6,0x1a1,0x0,0x5c3,0x0,0x4fef,0x0,0x5a1,0x180,0x580,0x180,0x2fe8,0x0,0x540,0x0,0x160,0x0,0x1fe3,0x4e0,0x4a0,0x140,0x120,0x0,0x0,0x0,0x2e0,0xfa0,0xa0,0x260,0x0,0xa0,0x0,0x0,0x60,0x960,0x0,0x1b60,0x0,0x900,0x0,0x40,0x0,0x40,0x0,0x8a0,0x0,0x2160,0x20,0x880,0x0,0x20,0x0,0x20,0x20,0x840,0x840,0x18c0,0x0,0x0,0x20,0x0,0x840,0x20,0x2060,0x1020,0x0,0x0,0x1020,0x0,0x3840,0x0,0x1820,0x800,0x2000,0x800,0x6820,0x0,0x2800,0x0,0x1000,0x0
};

const uint16_t buttonsIconsData[] = {
	10, 9,
	8,
	0,
	0,
	1, // color mode INDEX
	
	0x7705,0x7777,0x5750,0x3377,0x7577,0x7357,0x3777,0x5775,0x3373,0x7537,0x7357,0x3777,0x5775,0x7773,0x7537,0x7753,0x7777,0x5535,0x3333,0x5533,0x5505,0x5555,0x0550,0x5555,0x5055,0x7755,0x7777,0x5755,0x3377,0x7577,0x7357,0x3777,0x5775,0x3373,0x7537,0x7357,0x3777,0x5775,0x7773,0x7537,0x7755,0x7777,0x0555,0x5555,0x5055,0x7705,0x7777,0x5750,0x8878,0x7577,0x7857,0x8777,0x5775,0x8878,0x7577,0x7857,0x8777,0x5775,0x8878,0x7577,0x7758,0x7777,0x5585,0x8888,0x5588,0x5505,0x5555,0x0550,0x5555,0x5055,0x7755,0x7777,0x5755,0x8878,0x7577,0x7857,0x8777,0x5775,0x8878,0x7577,0x7857,0x8777,0x5775,0x8878,0x7577,0x7755,0x7777,0x0555,0x5555,0x5055,0x7705,0x7777,0x5750,0x4474,0x7547,0x7757,0x7777,0x5775,0x4474,0x7547,0x7757,0x7777,0x5775,0x4474,0x7547,0x7754,0x7777,0x5545,0x4444,0x5544,0x5505,0x5555,0x0550,0x5555,0x5055,0x7755,0x7777,0x5755,0x4474,0x7547,0x7757,0x7777,0x5775,0x4474,0x7547,0x7757,0x7777,0x5775,0x4474,0x7547,0x7755,0x7777,0x0555,0x5555,0x5055,0x7705,0x7777,0x5750,0x4477,0x7577,0x7457,0x4744,0x5775,0x4444,0x7544,0x7457,0x4747,0x5775,0x4774,0x7547,0x7754,0x7777,0x5545,0x4444,0x5544,0x5505,0x5555,0x0550,0x5555,0x5055,0x7755,0x7777,0x5755,0x4477,0x7577,0x7457,0x4744,0x5775,0x4444,0x7544,0x7457,0x4747,0x5775,0x4774,0x7547,0x7755,0x7777,0x0555,0x5555,0x5055
};

const uint16_t arrowsIconsData[] = {
	7,7,    // width, height
	8,      // number of frames
	0,      // frame looping speed
	0xCA30, // transparent color magenta
	0,      // color mode RGB
	//image data
	0xca30,0x5268,0xffff,0xffff,0xffff,0x5268,0xca30,0x5268,0xffff,0xffff,0xffff,0xffff,0xffff,0x5268,0x5268,0xffff,0x4439,0xffff,0x4439,0xffff,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0x4439,0xffff,0xffff,0xffff,0x4439,0x5268,0x5268,0x5268,0x4439,0x4439,0x4439,0x5268,0x5268,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0x5268,0x5268,0xffff,0xffff,0xffff,0x5268,0x5268,0x5268,0xffff,0xffff,0xffff,0xffff,0xffff,0x5268,0x5268,0xffff,0x4439,0xffff,0x4439,0xffff,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0x5268,0xffff,0xffff,0xffff,0x5268,0x5268,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0xca30,0x5268,0xffff,0xffff,0xffff,0x5268,0xca30,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0xffff,0x4439,0xffff,0xffff,0xffff,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0x4439,0xffff,0xffff,0xffff,0x4439,0x5268,0x5268,0x5268,0x4439,0x4439,0x4439,0x5268,0x5268,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0x5268,0x5268,0xffff,0xffff,0xffff,0x5268,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0xffff,0x4439,0xffff,0xffff,0xffff,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0x5268,0xffff,0xffff,0xffff,0x5268,0x5268,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0xca30,0x5268,0xffff,0xffff,0xffff,0x5268,0xca30,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0xffff,0xffff,0xffff,0x4439,0xffff,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0x4439,0xffff,0xffff,0xffff,0x4439,0x5268,0x5268,0x5268,0x4439,0x4439,0x4439,0x5268,0x5268,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0x5268,0x5268,0xffff,0xffff,0xffff,0x5268,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0xffff,0xffff,0xffff,0x4439,0xffff,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0x5268,0xffff,0xffff,0xffff,0x5268,0x5268,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0xca30,0x5268,0xffff,0xffff,0xffff,0x5268,0xca30,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0xffff,0x4439,0xffff,0x4439,0xffff,0x5268,0x5268,0xffff,0xffff,0xffff,0xffff,0xffff,0x5268,0x5268,0x4439,0xffff,0xffff,0xffff,0x4439,0x5268,0x5268,0x5268,0x4439,0x4439,0x4439,0x5268,0x5268,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30,0x5268,0x5268,0xffff,0xffff,0xffff,0x5268,0x5268,0x5268,0xffff,0xffff,0x4439,0xffff,0xffff,0x5268,0x5268,0xffff,0x4439,0xffff,0x4439,0xffff,0x5268,0x5268,0xffff,0xffff,0xffff,0xffff,0xffff,0x5268,0x5268,0x5268,0xffff,0xffff,0xffff,0x5268,0x5268,0xca30,0x5268,0x5268,0x5268,0x5268,0x5268,0xca30
};

const uint8_t gamebuinoLogo[] = {80,10,
	0b00111100,0b00111111,0b00111111,0b11110011,0b11110011,0b11110011,0b00110011,0b00111111,0b00111111,0b00011100,
	0b00111100,0b00111111,0b00111111,0b11110011,0b11110011,0b11110011,0b00110011,0b00111111,0b00111111,0b00100110,
	0b00110000,0b00110011,0b00110011,0b00110011,0b00000011,0b00110011,0b00110011,0b00110011,0b00110011,0b00100110,
	0b00110000,0b00110011,0b00110011,0b00110011,0b00000011,0b00110011,0b00110011,0b00110011,0b00110011,0b00101010,
	0b00110011,0b00111111,0b00110011,0b00110011,0b11110011,0b11000011,0b00110011,0b00110011,0b00110011,0b00011100,
	0b00110011,0b00111111,0b00110011,0b00110011,0b11110011,0b11000011,0b00110011,0b00110011,0b00110011,0b00000000,
	0b00110011,0b00110011,0b00110011,0b00110011,0b00000011,0b00110011,0b00110011,0b00110011,0b00110011,0b00000000,
	0b00110011,0b00110011,0b00110011,0b00110011,0b00000011,0b00110011,0b00110011,0b00110011,0b00110011,0b00000000,
	0b00111111,0b00110011,0b00110011,0b00110011,0b11110011,0b11110011,0b11110011,0b00110011,0b00111111,0b00000000,
	0b00111111,0b00110011,0b00110011,0b00110011,0b11110011,0b11110011,0b11110011,0b00110011,0b00111111,0b00000000,
};

const uint8_t homeIcons[] = {80,12,
	0b00000000,0b10000000,0b00000000,0b00000000,0b00000111,0b11111100,0b00000000,0b00000000,0b00011101,0b11000000,
	0b00001000,0b10000000,0b00000001,0b00000000,0b00000100,0b00001100,0b00001110,0b00000000,0b00111111,0b11100000,
	0b00000100,0b00001000,0b00000011,0b00010000,0b00000000,0b00111100,0b00011111,0b11111000,0b00110111,0b01100000,
	0b00000001,0b10010000,0b00000111,0b00001000,0b00001000,0b11111100,0b00100011,0b11000100,0b00111111,0b11100000,
	0b00000011,0b11000000,0b00111111,0b01000100,0b00000100,0b11111100,0b00100100,0b00100100,0b00011101,0b11001100,
	0b00110111,0b11100000,0b00111111,0b00100100,0b00111110,0b11111100,0b00111000,0b00011100,0b00100000,0b00110100,
	0b00000111,0b11101100,0b00111111,0b00100100,0b00000100,0b11111100,0b00111001,0b00011100,0b00100000,0b00100100,
	0b00000011,0b11000000,0b00111111,0b01000100,0b00001000,0b10111100,0b00111000,0b00011100,0b00100000,0b00100100,
	0b00001001,0b10000000,0b00000111,0b00001000,0b00000000,0b11111100,0b00111000,0b00011100,0b00100000,0b00110100,
	0b00010000,0b00100000,0b00000011,0b00010000,0b00000111,0b11111100,0b00111100,0b00111100,0b00100000,0b00101100,
	0b00000001,0b00010000,0b00000001,0b00000000,0b00000000,0b11110000,0b00011111,0b11111000,0b00011111,0b11000000,
	0b00000001,0b00000000,0b00000000,0b00000000,0b00000000,0b11000000,0b00000000,0b00000000,0b00000000,0b00000000,
};

const uint8_t volumeUnmuted[] = {8,12,
	0b00000000,
	0b00000000,
	0b00010000,
	0b00001000,
	0b01000100,
	0b00100100,
	0b00100100,
	0b01000100,
	0b00001000,
	0b00010000,
	0b00000000,
	0b00000000,
};

const uint16_t startSound[] = {0x0005,0x338,0x3FC,0x254,0x1FC,0x25C,0x3FC,0x368,0x0000};

void Gamebuino::begin() {
	// first we disable the watchdog timer so that we tell the bootloader everything is fine!
	WDT->CTRL.bit.ENABLE = 0;
	
	// let's to some sanity checks which are done on compile-time
	
	// check that the folder name length is at least 4 chars
#ifdef FOLDER_NAME
	static_assert(sizeof FOLDER_NAME - 1 >= 4, "your FOLDER_NAME must be at least 4 chars long!");
#else
	static_assert(sizeof __SKETCH_NAME__ - 1 >= 4 + 4, "Your sketch name is less than 4 chars long, please define FOLDER_NAME!");
	memcpy(folder_name, __SKETCH_NAME__, sizeof __SKETCH_NAME__ - 4);
	folder_name[sizeof __SKETCH_NAME__ - 4] = '\0';
#endif
	
	timePerFrame = 40; //25 FPS
	//nextFrameMillis = 0;
	//frameCount = 0;
	frameEndFlag = true;
	startMenuTimer = 255;

	//neoPixels
	neoPixels.begin();
	neoPixels.clear();

	//buttons
	buttons.begin();
	buttons.update();
	
	//tft
	tft.init();
	tft.setRotation(Rotation::down);
	
	
	display.fill(Color::black);
	display.fontSize = SYSTEM_DEFAULT_FONT_SIZE;
	
	display.setColor(Color::white);
	drawLogo(display, 2, 0, display.fontSize);
	display.setColor(Color::brown, Color::black);
	display.setCursor(0, display.height() - (display.getFontHeight()*display.fontSize));
	display.print("SD INIT... ");
	updateDisplay();
	
	if (!SD.begin(SD_CS)) {
		display.setColor(Color::red, Color::black);
		display.println("FAILED!");
		updateDisplay();
		delay(100);
	} else {
		display.setColor(Color::lightgreen, Color::black);
		display.println("OK!");
		updateDisplay();
	}

	display.setColor(Color::white, Color::black);
	display.fill(Color::black);
	
	// SD is initialized, let's switch to the folder!
	if (!SD.exists(folder_name)) {
		SD.mkdir(folder_name);
	}
	SD.chdir(folder_name);
	
	save = Save(&tft, SAVEFILE_NAME, folder_name);
	
	settings = Save(&tft, "/SETTINGS.SAV", "GBMS");
	settings.config(SETTINGSCONF_NUM_BLOCKS, settingsDefaults);
	
	//sound
	sound.begin();
	if (settings.get(SETTING_VOLUME_MUTE)) {
		sound.mute();
	}
	sound.setVolume(settings.get(SETTING_VOLUME));
	
	// language
	language.setCurrentLang((LangCode)settings.get(SETTING_LANGUAGE));
	
	// neoPixels
	neoPixels.setBrightness(neoPixelsIntensities[settings.get(SETTING_NEOPIXELS_INTENSITY)]);
	
	Graphics_SD::setTft(&tft);
	// only do titleScreen after a hard power on
	if (PM->RCAUSE.bit.POR) {
		startScreen();
#if AUTOSHOW_TITLESCREEN
		titleScreen();
#endif
	}
	pickRandomSeed();
	display.clear();
}

void Gamebuino::drawLogo(Graphics& g, int8_t x, int8_t y, uint8_t scale) {
	Image logo(78, 10, ColorMode::index);
	logo.clear();
	logo.drawBitmap(-2, 0, gamebuinoLogo);
	g.drawImage(x*scale, y*scale, logo, 78*scale, 10*scale);
}

void Gamebuino::startScreen(){
	Image startLights(startLightsData);
	int8_t i = 24;
	update();
	sound.play(startSound);
	display.fontSize = SYSTEM_DEFAULT_FONT_SIZE;
	while(i){
		while(!update());
		i--;
		display.clear();
		Image logo(78, 10, ColorMode::index);
		drawLogo(display, 2, min(0, i-10), display.fontSize);
		lights.drawImage(0, 0, startLights);
	}
	lights.clear();
	update();
}

void Gamebuino::titleScreen() {
	ColorMode ts_backup_colorMode = display.colorMode;
	uint16_t ts_backup_width = display._width;
	uint16_t ts_backup_height = display._height;
	display.fontSize = SYSTEM_DEFAULT_FONT_SIZE;
	char filename[17] = "TITLESCREEN.BMP";
	
	bool titleScreenImageExists = SD.exists(filename);
	bool displayName = !titleScreenImageExists;
	if (!titleScreenImageExists && SD.exists("REC")) {
		strcpy(filename, "REC/");
		const uint8_t f_offset = 4;
		File dir_walk = SD.open(filename);
		File entry;
		while (entry = dir_walk.openNextFile()) {
			if (!entry.isFile()) {
				continue;
			}
			entry.getName(filename + f_offset, sizeof(filename) - f_offset);
			if (!strstr(filename, ".GMV") && !strstr(filename, ".gmv")) {
				continue;
			}
			titleScreenImageExists = true;
			displayName = true;
			break;
		}
	}
	if (titleScreenImageExists) {
		display.init(ts_backup_width, ts_backup_height, filename);
	}
	
	bool first = true;
	bool reInitDisplay = false;
	
	Image buttonsIcons = Image(buttonsIconsData);
	
	while(1) {
		while(!update());
		
		if (titleScreenImageExists) {
			display.nextFrame();
		} else {
			display.clear();
		}
		
		if (displayName) {
			// center bar
			display.setColor(BROWN);
			display.fillRect(0, 15*display.fontSize, 80*display.fontSize, 9*display.fontSize);
			
			// game name
			display.setColor(WHITE);
			display.setCursor(2*display.fontSize, 17*display.fontSize);
			display.println(folder_name);
		}
		
		//blinking border
		gb.display.setColor((gb.frameCount % 8) >= 4 ? BROWN : BLACK);
		gb.display.drawRect(0, 0, gb.display.width(), gb.display.height());
		
		//blinking A button icon
		if((gb.frameCount%8) >= 4){
			buttonsIcons.setFrame(1); //button A pressed
		} else {
			buttonsIcons.setFrame(0); //button A released
		}
		uint8_t scale = gb.display.width() == 80 ? 1 : 2;
		uint8_t w = buttonsIcons.width() * scale;
		uint8_t h = buttonsIcons.height() * scale;
		uint8_t x = gb.display.width() - w;
		uint8_t y = gb.display.height() - h;
		gb.display.drawImage(x, y, buttonsIcons, w, h);
		
		if (gb.buttons.pressed(Button::a)) {
			sound.playOK();
			break;
		}
	}
	display.init(ts_backup_width, ts_backup_height, ts_backup_colorMode);
}

bool recording_screen = false;

bool Gamebuino::update() {
	if (((nextFrameMillis - millis()) > timePerFrame) && frameEndFlag) { //if time to render a new frame is reached and the frame end has ran once
		nextFrameMillis = millis() + timePerFrame;
		frameCount++;

		frameEndFlag = false;
		frameStartMicros = micros();

		buttons.update();

		return true;

	}
	if (frameEndFlag) {
		return false;
	}
	// ok, here is the first time after a frame, so we'll better check stuff correctly
	
	//Home menu
	checkHomeMenu();
	
	//draw and update popups
	updatePopup();
	
	sound.update(); // update sound stuff once per frame
	
	
	//send buffer to the screen
	updateDisplay();
	
	//neoPixels update
	uint8_t px_height = lights.height();
	uint8_t px_width = lights.width();
	const uint8_t px_map[] = {
		7, 0,
		6, 1,
		5, 2,
		4, 3,
	};
	for (uint8_t y = 0; y < px_height; y++) {
		for (uint8_t x = 0; x < px_width; x++) {
			RGB888 c = rgb565Torgb888(lights.getPixel(x, y));
			// intensity is scaled directly via neoPixels.setBrightness
			neoPixels.setPixelColor(px_map[y*px_width + x], c.r, c.g, c.b);
		}
	}
	//show a red blinking pixel on recording screen
	if (recording_screen) {
		neoPixels.setPixelColor(px_map[0], (frameCount % 10) < 5 ? 0xFF : 0, 0, 0);
	}
	neoPixels.show();
	neoPixels.clear();

	frameDurationMicros = micros() - frameStartMicros;
	Graphics_SD::update(); // update screen recordings
	
	frameEndFlag = true; // we are at end of frame
	return false;
}

void Gamebuino::updateDisplay() {
	tft.drawImage(0, 0, display, tft.width(), tft.height()); //send the buffer to the screen
}

void Gamebuino::setFrameRate(uint8_t fps) {
	timePerFrame = 1000 / fps;
}

void Gamebuino::pickRandomSeed(){
	randomSeed(micros() * micros() ^ analogRead(1)*analogRead(2)); // can't use analogRad(0) as we have a speaker attached there
}

uint8_t Gamebuino::getCpuLoad(){
	return(frameDurationMicros/(10*timePerFrame));
}

extern "C" char* sbrk(int incr);	
uint16_t Gamebuino::getFreeRam() {	
	// from https://github.com/mpflaga/Arduino-MemoryFree/blob/master/MemoryFree.cpp
	char top;
	return &top - reinterpret_cast<char*>(sbrk(0));
}

int8_t Gamebuino::menu(const char* const* items, uint8_t length) {
	//display.persistence = false;
	int8_t activeItem = 0;
	int8_t currentY = display.height();
	int8_t targetY = 0;
	bool exit = false;
	int8_t answer = -1;
	while (1) {
		while(!update());
		display.setColor(WHITE);
		display.fill();
		display.setColor(BLACK, WHITE);
		if (buttons.pressed(Button::a) || buttons.pressed(Button::b) || buttons.pressed(Button::menu)) {
			exit = true; //time to exit menu !
			targetY = - display.fontHeight * length - 2; //send the menu out of the screen
			if (buttons.pressed(Button::a)) {
				answer = activeItem;
				sound.playOK();
			} else {
				sound.playCancel();
			}
		}
		if (exit == false) {
			if (buttons.repeat(Button::down,4)) {
				activeItem++;
				sound.playTick();
			}
			if (buttons.repeat(Button::up,4)) {
				activeItem--;
				sound.playTick();
			}
			//don't go out of the menu
			if (activeItem == length) activeItem = 0;
			if (activeItem < 0) activeItem = length - 1;

			targetY = -display.fontHeight * activeItem + (display.fontHeight+4); //center the menu on the active item
		} else { //exit :
			if ((currentY - targetY) <= 1)
			return (answer);
		}
		//draw a fancy menu
		currentY = (currentY + targetY) / 2;
		display.cursorX = 0;
		display.cursorY = currentY;
		display.fontSize = SYSTEM_DEFAULT_FONT_SIZE;
		display.textWrap = false;
		for (byte i = 0; i < length; i++) {
			if (i == activeItem){
				display.cursorX = 3;
				display.cursorY = currentY + display.fontHeight * activeItem;
			}
			display.println((const char*)items[i]);
		}

		//display.fillRect(0, currentY + 3 + 8 * activeItem, 2, 2, BLACK);
#if DISPLAY_MODE == DISPLAY_MODE_RGB565
		display.setColor(Color::white);
#else
		display.setColor(ColorIndex::white);
#endif
		display.drawFastHLine(0, currentY + display.fontHeight * activeItem - 1, display.width());
#if DISPLAY_MODE == DISPLAY_MODE_RGB565
		display.setColor(Color::black);
#else
		display.setColor(ColorIndex::black);
#endif
		display.drawRoundRect(0, currentY + display.fontHeight * activeItem - 2, display.width(), (display.fontHeight+3), 3);
	}
}

#define HOME_MENU_SAVE_STATE \
	int16_t hm_save_cursorX = display.cursorX; \
	int16_t hm_save_cursorY = display.cursorY; \
	Color hm_save_color = display.color.c; \
	Color hm_save_bgcolor = display.bgcolor.c; \
	uint8_t hm_save_fontSize = display.fontSize; \
	bool hm_save_textWrap = display.textWrap; \
	uint8_t* hm_save_font = display.font; \
	uint8_t hm_save_fontWidth = display.fontWidth; \
	uint8_t hm_save_fontHeight = display.fontHeight; \
	Rotation hm_save_rotation = tft.getRotation(); \
	tft.setRotation(Rotation::down);

#define HOME_MENU_RESTORE_STATE \
	display.cursorX = hm_save_cursorX; \
	display.cursorY = hm_save_cursorY; \
	display.color.c = hm_save_color; \
	display.bgcolor.c = hm_save_bgcolor; \
	display.fontSize = hm_save_fontSize; \
	display.textWrap = hm_save_textWrap; \
	display.font = hm_save_font; \
	display.fontWidth = hm_save_fontWidth; \
	display.fontHeight = hm_save_fontHeight; \
	tft.setRotation(hm_save_rotation);

void Gamebuino::checkHomeMenu() {
	//get back to game list when "HOME is held
	if (buttons.held(Button::home, 25)){
		changeGame();
	}
	if (framesDisplayRecording != -1) {
		framesDisplayRecording--;
	}
	if (buttons.released(Button::home) || (framesDisplayRecording == 0 && recording_screen)) {
		if (recording_screen) {
			// stop the recording
			HOME_MENU_SAVE_STATE;
			sound.startEfxOnly();
			bool isMute = sound.isMute();
			sound.mute();
			display.setFont(font3x5);
			neoPixels.clear();
			neoPixels.show();
			display.stopRecording(true);
			recording_screen = false;
			//refresh screen to erase log messages
			updateDisplay();
			if (!isMute) {
				sound.unmute();
			}
			sound.stopEfxOnly();
			HOME_MENU_RESTORE_STATE;
		}
		homeMenu();
	}
}

void Hook_ExitHomeMenu() __attribute__((weak));
void Hook_ExitHomeMenu() {
	
}

bool homeMenuGetUniquePath(char* name, uint8_t offset, uint8_t len) {
	if(!SD.exists("REC")) {
		SD.mkdir("REC");
	}
	uint32_t start;
	File cache;
	if (!SD.exists("REC/REC.CACHE")) {
		cache = SD.open("REC/REC.CACHE", FILE_WRITE);
		cache.rewind();
		f_write32(0, &cache); // images
		cache.close();
	}
	cache = SD.open("REC/REC.CACHE", FILE_WRITE);
	cache.rewind();
	start = f_read32(&cache);
	start = sdPathNoDuplicate(name, offset, len, start + 1);
	cache.rewind();
	if (start == -1) {
		f_write32(0, &cache);
		start = sdPathNoDuplicate(name, offset, len);
	}
	if (start == -1) {
		cache.close();
		return false;
	}
	f_write32(start, &cache);
	cache.close();
	return true;
}

template<uint8_t N>
void fileEndingGmvToBmp(char (&name)[N]) {
	name[N-4] = 'B';
	//name[N-3] = 'M'; // no need as GMV and BMP have same second letter
	name[N-2] = 'P';
}

void Gamebuino::homeMenu(){
	//here we don't use gb.update and display.not to interfere with the game
	//the only things we use are gb.tft and gb.buttons
	sound.startEfxOnly();
	
	HOME_MENU_SAVE_STATE;
	
	const int numItems = 5;
	int currentItem = 2; //default on center item
	unsigned long lastMillis = 0;
	//3 text lines vertical coordinates
	const int yOffset = 52;
	bool changed = true;
	int frameCounter = 0;
	
	neoPixels.clear();
	neoPixels.show();
	// determine the neoPixel color index
	uint8_t neoPixelsIntensity = 0;
	for (;(neoPixelsIntensity < 5) && (neoPixels.getBrightness() > neoPixelsIntensities[neoPixelsIntensity]);neoPixelsIntensity++);
	
	//static screen content
	//draw icons first as it's where the user is focused
	{ //local scope to free RAM used by the buffer
		tft.setColor(DARKGRAY);
		tft.fillRect(0, yOffset, 160, 32);
		tft.setColor(WHITE);
		Image icons(80, 12, ColorMode::index);
		icons.fill(DARKGRAY);
		icons.setColor(WHITE);
		icons.drawBitmap(0, 0, homeIcons);
		tft.drawImage(0, yOffset + 4, icons, 80*2, 12*2);
		//erase soundwaves if muted
		if (!sound.getVolume() || sound.isMute()) {
			tft.setColor(DARKGRAY);
			tft.fillRect(50, yOffset + 8, 10, 16);
		}
	}
	
	//logo
	tft.setColor(BLACK);
	tft.fillRect(0, 0, 160, 28);
	drawLogo(tft, 2, 1, 2);

	//horizontal stripes top
	tft.setColor(DARKGRAY);
	for (int i = 28; i < yOffset; i+=4){
		tft.fillRect(0, i, tft.width(), 2);
	}
	
	//horizontal stripes bottom
	for (int i = yOffset + 34; i < tft.height(); i+=4){
		tft.fillRect(0, i, tft.width(), 2);
	}
	
	while(1) {
		//Ensure constant framerate using millis (40ms = 25FPS)
		while(!((millis() - lastMillis) > 40));
		
		sound.update(); // we still need sound...
		lastMillis = millis();
		buttons.update();
		frameCounter++;
		
		//clear noPixels
		neoPixels.clear();
		
		if (buttons.released(Button::home) || buttons.released(Button::b) || buttons.released(Button::menu)) {
			sound.stopEfxOnly();
			HOME_MENU_RESTORE_STATE;
			Hook_ExitHomeMenu();
			return;
		}
		
		if (buttons.repeat(Button::right, 8) && (currentItem < (numItems - 1))) {
			currentItem++;
			changed = true;
		}
		
		if (buttons.repeat(Button::left, 8) && (currentItem > 0)) {
			currentItem--;
			changed = true;
		}
		
		switch(currentItem){
			//// NEOPIXELS
			case 0:
				if (buttons.repeat(Button::up, 4) && (neoPixelsIntensity < 4)){
					neoPixelsIntensity ++;
					changed = true;
				}
				if (buttons.repeat(Button::down, 4) && (neoPixelsIntensity > 0)){
					neoPixelsIntensity--;
					changed = true;
				}
				//toggle with "A"
				if (buttons.released(Button::a)){
					if(neoPixelsIntensity > 0){
						neoPixelsIntensity = 0;
					} else {
						neoPixelsIntensity = 4;
					}
					changed = true;
				}
				//update and save settings
				if(changed == true){
					neoPixels.setBrightness(neoPixelsIntensities[neoPixelsIntensity]);
					settings.set(SETTING_NEOPIXELS_INTENSITY, neoPixelsIntensity);
				}
				//light up neopixels according to intensity
				for(uint8_t i = 0; i < neoPixels.numPixels(); i++){
					neoPixels.setPixelColor(i, 0xFF, 0xFF, 0xFF);
				}
				break;
			////VOLUME
			case 1:
				//toggle mute/unmute
				if (buttons.released(Button::a)) {
					if (sound.isMute()) {
						sound.unmute();
						settings.set(SETTING_VOLUME_MUTE, (int32_t)0);
						if (!sound.getVolume()) {
							sound.setVolume(6);
							settings.set(SETTING_VOLUME, 6);
						}
					} else if (sound.getVolume()) {
						sound.mute();
						settings.set(SETTING_VOLUME_MUTE, 1);
					} else {
						sound.setVolume(6);
						settings.set(SETTING_VOLUME, 6);
					}
					changed = true;
				}
				//increase volume
				if ((buttons.repeat(Button::up, 4) && (sound.getVolume() < 8))) {
					if (sound.isMute()) {
						sound.unmute();
						settings.set(SETTING_VOLUME_MUTE, (int32_t)0);
						sound.setVolume(1);
					} else {
						sound.setVolume(sound.getVolume() + 1);
					}
					settings.set(SETTING_VOLUME, sound.getVolume());
					changed = true;
				}
				//reduce volume
				if (buttons.repeat(Button::down, 4) && sound.getVolume() && !sound.isMute()) {
					sound.setVolume(sound.getVolume() - 1);
					settings.set(SETTING_VOLUME, sound.getVolume());
					changed = true;
				}
				
				if (changed) {
					sound.playTick();
				}
				break;
			////EXIT
			case 2:
				if (buttons.pressed(Button::a)) {
					changeGame();
				}
				break;
			////SCREENSHOT
			case 3:
				if (buttons.released(Button::a)) {
					tft.setColor(WHITE);
					tft.drawRect(currentItem*32, yOffset, 32, 32);
					tft.drawRect(1 + currentItem*32, yOffset+1, 30, 30);
					
					char name[] = "REC/00000.GMV";
					// now `name` will be a unique thing
					// 9 because "REC/" is 4 long, 5 because "00000" is 4 chars
					bool success = homeMenuGetUniquePath(name, 4, 5);
					if (success) {
						fileEndingGmvToBmp(name);
						success = display.save(name);
					}
					// we temp. set inited to false so that delay() won't re-draw the screen
					inited = false;
					if (success) {
						tft.setColor(LIGHTGREEN);
						tft.drawRect(currentItem*32, yOffset, 32, 32);
						tft.drawRect(1 + currentItem*32, yOffset+1, 30, 30);
						delay(400);
						changed = true;
					} else {
						tft.setColor(RED);
						tft.drawRect(currentItem*32, yOffset, 32, 32);
						tft.drawRect(1 + currentItem*32, yOffset+1, 30, 30);
						delay(400);
						changed = true;
					}
					inited = true;
				}
				break;
			////RECORD SCREEN
			case 4:
				if (buttons.released(Button::a) || buttons.held(Button::a, 25)) {
					tft.setColor(WHITE	);
					tft.drawRect(currentItem*32, yOffset, 32, 32);
					tft.drawRect(1 + currentItem*32, yOffset+1, 30, 30);
					char name[] = "REC/00000.GMV";
					bool success = homeMenuGetUniquePath(name, 4, 5);
					bool infinite = buttons.held(Button::a, 25);
					if (success) {
						if (!infinite) {
							fileEndingGmvToBmp(name);
							framesDisplayRecording = (1000 / timePerFrame) * 3;
						} else {
							framesDisplayRecording = -1;
						}
						success = display.startRecording(name);
					}
					// we temp. set inited to false so that delay() won't re-draw the screen
					inited = false;
					if (success) {
						recording_screen = true;
						if (infinite) {
							do {
								delay(10);
								buttons.update();
							} while (!buttons.released(Button::a));
						}
						tft.setColor(LIGHTGREEN);
						tft.drawRect(currentItem*32, yOffset, 32, 32);
						tft.drawRect(1 + currentItem*32, yOffset+1, 30, 30);
						delay(400);
						inited = true;
						sound.stopEfxOnly();
						HOME_MENU_RESTORE_STATE;
						Hook_ExitHomeMenu();
						return;
					} else {
						tft.setColor(RED);
						tft.drawRect(currentItem*32, yOffset, 32, 32);
						tft.drawRect(1 + currentItem*32, yOffset+1, 30, 30);
						delay(400);
						changed = true;
					}
					inited = true;
				}
				break;
		}
		
		//draw blinking selector
		tft.setColor((frameCounter % 8) >= 4 ? BROWN : DARKGRAY);
		tft.drawRect(currentItem*32, yOffset, 32, 32);
		tft.drawRect(1 + currentItem*32, yOffset+1, 30, 30);
		
		
		if (changed) {
			//erase previous selector position
			tft.setColor(DARKGRAY);
			if (buttons.repeat(Button::left, 8)) {
				tft.drawRect((currentItem+1)*32, yOffset, 32, 32);
				tft.drawRect(1 + (currentItem+1)*32, yOffset+1, 30, 30);
			}
			if (buttons.repeat(Button::right, 8)) {
				tft.drawRect((currentItem-1)*32, yOffset, 32, 32);
				tft.drawRect(1 + (currentItem-1)*32, yOffset+1, 30, 30);
			}
			
				
			////VOLUME
			if (currentItem == 1) {
				if (sound.getVolume() && !sound.isMute()) {
					Image volume(8, 12, ColorMode::index);
					volume.clear();
					volume.fill(DARKGRAY);
					volume.setColor(WHITE);
					volume.drawBitmap(0, 0, volumeUnmuted);
					tft.drawImage(48, yOffset + 4, volume, 8*2, 12*2);
				} else { //erase waveform if muted
					tft.setColor(DARKGRAY);
					tft.fillRect(50, yOffset + 8, 10, 16);
				}
			}
		}
		
		//draw light level
		if ((currentItem == 0) && neoPixelsIntensity) {			
			tft.setColor(WHITE);
			int lightHeight = neoPixelsIntensity * 32 / 4;
			tft.drawRect(currentItem*32 + 30, yOffset + (32 - lightHeight), 2, lightHeight);
		}
		
		//draw volume level
		if ((currentItem == 1) && (sound.getVolume())) {			
			tft.setColor(WHITE);
			int volumeHeight = sound.getVolume() * 32 / 8;
			tft.drawRect(currentItem*32 + 30, yOffset + (32 - volumeHeight), 2, volumeHeight);
		}
		
		//updated neopixels
		neoPixels.show();
		
		changed = false;
	}
}

#define KEYBOARD_W 16
#define KEYBOARD_H 14
void Gamebuino::keyboard(char* text, uint8_t length) {
	//display.persistence = false;
	//memset(text, 0, length); //clear the text
	text[length-1] = '\0';
	//active character in the typing area
	int8_t activeChar = 0;
	//selected char on the keyboard
	int8_t activeX = 0;
	int8_t activeY = 2;
	//position of the keyboard on the screen
	int8_t currentX = display.width();
	int8_t currentY = display.height();
	int8_t targetX = 0;
	int8_t targetY = 0;

	while (1) {
		while(!update());
		display.clear();
		//move the character selector
		if (buttons.repeat(Button::down, 4)) {
			activeY++;
			sound.playTick();
		}
		if (buttons.repeat(Button::up, 4)) {
			activeY--;
			sound.playTick();
		}
		if (buttons.repeat(Button::right, 4)) {
			activeX++;
			sound.playTick();
		}
		if (buttons.repeat(Button::left, 4)) {
			activeX--;
			sound.playTick();
		}
		//don't go out of the keyboard
		if (activeX == KEYBOARD_W) activeX = 0;
		if (activeX < 0) activeX = KEYBOARD_W - 1;
		if (activeY == KEYBOARD_H) activeY = 0;
		if (activeY < 0) activeY = KEYBOARD_H - 1;
		//set the keyboard position on screen
		targetX = -(display.fontWidth+1) * activeX + display.width() / 2 - 3;
		targetY = -(display.fontHeight+1) * activeY + display.height() / 2 - 4 - display.fontHeight;
		//smooth the keyboard displacement
		currentX = (targetX + currentX) / 2;
		currentY = (targetY + currentY) / 2;
		//type character
		if (buttons.pressed(Button::a)) {
			if (activeChar < (length-1)) {
				byte thisChar = activeX + KEYBOARD_W * activeY;
				if (thisChar >= 0x80) {
					thisChar += 0x20;
				}
				if((thisChar == 0)||(thisChar == 10)||(thisChar == 13)) //avoid line feed and carriage return
				continue;
				text[activeChar] = thisChar;
				text[activeChar+1] = '\0';
			}
			activeChar++;
			sound.playOK();
			if (activeChar > length)
			activeChar = length;
		}
		//erase character
		if (buttons.pressed(Button::b)) {
			activeChar--;
			sound.playCancel();
			if (activeChar >= 0)
			text[activeChar] = 0;
			else
			activeChar = 0;
		}
		//leave menu
		if (buttons.pressed(Button::menu)) {
			sound.playOK();
			while (1) {
				if (update()) {
					display.clear();
					//display.setCursor(0,0);
					display.println("You entered\n");
					display.print(text);
					display.println("\n\n\n\x15:okay \x16:edit");
					if(buttons.pressed(Button::a)){
						sound.playOK();
						return;
					}
					if(buttons.pressed(Button::b)){
						sound.playCancel();
						break;
					}
				}
			}
		}
		//draw the keyboard
		for (int8_t y = 0; y < KEYBOARD_H; y++) {
			for (int8_t x = 0; x < KEYBOARD_W; x++) {
				byte c = x + y * KEYBOARD_W;
				if (c >= 0x80) {
					c += 0x20;
				}
				display.drawChar(currentX + x * (display.fontWidth+1), currentY + y * (display.fontHeight+1), c, 1);
			}
		}
		//draw instruction
		display.cursorX = currentX-display.fontWidth*7-2;
		display.cursorY = currentY+1*(display.fontHeight+1);
		display.print("\25");
		display.print(language._get(lang_keyboard_type));
		
		display.cursorX = currentX-display.fontWidth*7-2;
		display.cursorY = currentY+2*(display.fontHeight+1);
		display.print("\26");
		display.print(language._get(lang_keyboard_back));
		
		display.cursorX = currentX-display.fontWidth*7-2;
		display.cursorY = currentY+3*(display.fontHeight+1);
		display.print("\27");
		display.print(language._get(lang_keyboard_save));
		
		//erase some pixels around the selected character
		display.setColor(DISPLAY_DEFAULT_BACKGROUND_COLOR);
		display.drawFastHLine(currentX + activeX * (display.fontWidth+1) - 1, currentY + activeY * (display.fontHeight+1) - 2, 7);
		//draw the selection rectangle
		display.setColor(DISPLAY_DEFAULT_COLOR);
		display.drawRoundRect(currentX + activeX * (display.fontWidth+1) - 2, currentY + activeY * (display.fontHeight+1) - 3, (display.fontWidth+2)+(display.fontWidth-1)%2, (display.fontHeight+5), 3);
		//draw keyboard outline
		//display.drawRoundRect(currentX - 6, currentY - 6, KEYBOARD_W * (display.fontWidth+1) + 12, KEYBOARD_H * (display.fontHeight+1) + 12, 8, BLACK);
		//text field
		display.drawFastHLine(0, display.height()-display.fontHeight-2, display.width());
		display.setColor(DISPLAY_DEFAULT_BACKGROUND_COLOR);
		display.fillRect(0, display.height()-display.fontHeight-1, display.width(), display.fontHeight+1);
		//typed text
		display.cursorX = 0;
		display.cursorY = display.height()-display.fontHeight;
		display.setColor(DISPLAY_DEFAULT_COLOR);
		display.print(text);
		//blinking cursor
		if (((frameCount % 8) < 4) && (activeChar < (length-1))) {
			display.drawChar(display.fontWidth * activeChar, display.height()-display.fontHeight, '_',1);
		}
	}
}

void Gamebuino::popup(const char* text, uint8_t duration){
	popupText = text;
	popupTimeLeft = duration+12;
}

void Gamebuino::updatePopup(){
	if (popupTimeLeft){
		uint8_t yOffset = 0;
		if(popupTimeLeft<12){
			yOffset = 12-popupTimeLeft;
		}
		display.setFontSize(1);
		display.setColor(DISPLAY_DEFAULT_BACKGROUND_COLOR);
		display.fillRoundRect(0,display.height()-display.getFontHeight()+yOffset-3,display.width(),display.getFontHeight()+3,3);
		display.setColor(DISPLAY_DEFAULT_COLOR);
		display.drawRoundRect(0,display.height()-display.getFontHeight()+yOffset-3,display.width(),display.getFontHeight()+3,3);
		display.setCursor(
			4,
			display.height()-display.getFontHeight()+yOffset-1
		);
		display.print(popupText);
		popupTimeLeft--;
	}
}

void Gamebuino::changeGame(){
	//clear the screen
	tft.fill(Color::black);
	//flash loader.bin
	bootloader.loader();
}

void Gamebuino::getDefaultName(char* string){
	settings.get(SETTING_DEFAULTNAME, string, 13);
}

bool Gamebuino::collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h){
	if((x1>=x2)&&(x1<x2+w))
	if((y1>=y2)&&(y1<y2+h))
	return true;
	return false;
}

bool Gamebuino::collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2){
  return !( x2     >=  x1+w1  || 
            x2+w2  <=  x1     || 
            y2     >=  y1+h1  ||
            y2+h2  <=  y1     );
}

bool Gamebuino::collideBitmapBitmap(int16_t x1, int16_t y1, const uint8_t* b1, int16_t x2, int16_t y2, const uint8_t* b2){
  int16_t w1 = b1[0];
  int16_t h1 = b1[1];
  int16_t w2 = b2[0];
  int16_t h2 = b2[1];

  if(collideRectRect(x1, y1, w1, h1, x2, y2, w2, h2) == false){
    return false;
  }
  
  int16_t xmin = (x1>=x2)? 0 : x2-x1;
  int16_t ymin = (y1>=y2)? 0 : y2-y1;
  int16_t xmax = (x1+w1>=x2+w2)? x2+w2-x1 : w1;
  int16_t ymax = (y1+h1>=y2+h2)? y2+h2-y1 : h1;

  for(uint8_t y = ymin; y < ymax; y++){
    for(uint8_t x = xmin; x < xmax; x++){
      if(display.getBitmapPixel(b1, x, y) && display.getBitmapPixel(b2, x1+x-x2, y1+y-y2)){
        return true;
      }
    }
  }

  return false;
}

Color Gamebuino::createColor(uint8_t r, uint8_t g, uint8_t b) {
	return (Color)rgb888Torgb565({r, g, b});
}

uint8_t Gamebuino::getTimePerFrame() {
	return timePerFrame;
}

void Gamebuino::setScreenRotation(Rotation r) {
	Rotation rr = tft.getRotation();
	if (((uint8_t)r - (uint8_t)rr) % 2) {
		// we need to swap width/height of gb.display
		int16_t tmp = gb.display._width;
		gb.display._width = gb.display._height;
		gb.display._height = tmp;
	}
	tft.setRotation(r);
}

Rotation Gamebuino::getScreenRotation() {
	return tft.getRotation();
}

int8_t tone_identifier = -1;

} // namespace Gamebuino_Meta

#ifndef GAMEBUINO_COMPAT_MODE
Gamebuino gb;
#endif

void tone(uint32_t outputPin, uint32_t frequency, uint32_t duration) {
	if (Gamebuino_Meta::tone_identifier != -1) {
		gb.sound.stop(Gamebuino_Meta::tone_identifier);
	}
	Gamebuino_Meta::tone_identifier = gb.sound.tone(frequency, duration);
}

void noTone(uint32_t outputPin) {
	gb.sound.stop(Gamebuino_Meta::tone_identifier);
	Gamebuino_Meta::tone_identifier = -1;
}

void yield() {
	if (gb.inited && (gb.frameEndFlag || gb.frameStartMicros)) {
		gb.update();
	}
}
