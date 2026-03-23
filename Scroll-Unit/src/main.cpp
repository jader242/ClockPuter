#include <Arduino.h>
#include <M5UnitScroll.h>
#include <M5Cardputer.h>
#include <WiFi.h>
#include <time.h>

M5UnitScroll scroll;

int firstRun = 0;
char ssid[33] = {0};
char password[65] = {0};
char timeOffset[4] = {0};

void setSSID() {
	memset(ssid, 0, sizeof(ssid));
	bool done = false;
	while (!done) {
		M5Cardputer.update();
		M5Cardputer.Display.setCursor(0, 0);
		M5Cardputer.Display.println("Enter SSID: ");
		auto status = M5Cardputer.Keyboard.keysState();
		size_t len = strlen(ssid);
		for (char i : status.word) {
			if (len < sizeof(ssid) - 1) {
				ssid[len] = i;
				ssid[len + 1] = '\0';
			}
		}
		if (status.del && len > 0) {
			ssid[len - 1] = '\0';
			M5Cardputer.Display.fillRect(0, 20, 240, 115);
		}
		if (status.enter && len > 0) {
			done = true;
		}else if (status.enter && len <= 0) {
			M5Cardputer.Display.fillScreen(BLACK);
		}
		M5Cardputer.Display.setCursor(0, 20);
		M5Cardputer.Display.println(ssid);
		delay(200);
	}
	M5Cardputer.Display.fillScreen(BLACK);
	M5Cardputer.Display.setCursor(0, 0);
	M5Cardputer.Display.println("SSID set to:");
	M5Cardputer.Display.println(ssid);
	delay(2000);
	M5Cardputer.Display.fillScreen(BLACK);
}

void setPassword() {
	memset(password, 0, sizeof(password));
	bool done = false;
	while (!done) {
		M5Cardputer.update();
		M5Cardputer.Display.setCursor(0, 0);
		M5Cardputer.Display.println("Enter password:");
		auto status = M5Cardputer.Keyboard.keysState();
		size_t len = strlen(password);
		for (char i : status.word) {
			if (len < sizeof(password) - 1) {
				password[len] = i;
				password[len + 1] = '\0';
			}
		}
		if (status.del && len > 0) {
			password[len - 1] = '\0';
			M5Cardputer.Display.fillRect(0, 20, 240, 115);
		}
		if (status.enter && len > 0) {
			done = true;
		}else if (status.enter && len <= 0) {
			M5Cardputer.Display.fillScreen(BLACK);
		}
		M5Cardputer.Display.setCursor(0, 20);
		M5Cardputer.Display.println(password);
		delay(200);
	}
	M5Cardputer.Display.fillScreen(BLACK);
	M5Cardputer.Display.setCursor(0, 0);
	M5Cardputer.Display.println("Password set to:");
	M5Cardputer.Display.println(password);
	delay(2000);
	M5Cardputer.Display.fillScreen(BLACK);
}

void setTimeOffset() {
	memset(timeOffset, 0, sizeof(timeOffset));
	bool done = false;
	while (!done) {
		M5Cardputer.update();
		M5Cardputer.Display.setCursor(0, 0);
		M5Cardputer.Display.println("Enter UTC Offset:");
		auto status = M5Cardputer.Keyboard.keysState();
		size_t len = strlen(timeOffset);
		for (char i : status.word) {
			if ((i >= '0' && i <= '9') || (i == '-' && len == 0)) {
				timeOffset[len] = i;
				timeOffset[len + 1] = '\0';
			}
		}
		if (status.del && len > 0) {
			timeOffset[len - 1] = '\0';
			M5Cardputer.Display.fillRect(0, 20, 240, 115);
		}
		if (status.enter && len > 0) {
			int offset = atoi(timeOffset);
			if (offset <= 14 && offset >= -12) {
				done = true;
			}else {
				M5Cardputer.Display.fillScreen(BLACK);
				memset(timeOffset, 0, sizeof(timeOffset));
				M5Cardputer.Display.drawString("Invalid UTC offset", 0, 0);
				delay(2000);
				M5Cardputer.Display.fillScreen(BLACK);
			}
		}else if (status.enter && len <= 0) {
			M5Cardputer.Display.fillScreen(BLACK);
		}
		M5Cardputer.Display.setCursor(0, 20);
		M5Cardputer.Display.println(timeOffset);
		delay(200);
	}
	M5Cardputer.Display.fillScreen(BLACK);
	M5Cardputer.Display.setCursor(0, 0);
	M5Cardputer.Display.println("Timezone set to:");
	M5Cardputer.Display.println(timeOffset);
	delay(2000);
	M5Cardputer.Display.fillScreen(BLACK);
}

void drawBattery() {
	int percentage = M5Cardputer.Power.getBatteryLevel();
	M5Cardputer.Display.setTextSize(2);
	char batteryStr[5];
	snprintf(batteryStr, sizeof(batteryStr), "%d%%", percentage);
	uint8_t textWidth = M5Cardputer.Display.textWidth(batteryStr);
	uint8_t fullWidth = M5Cardputer.Display.textWidth("100%");
	if (percentage <= 50) {
		M5Cardputer.Display.fillRect(((M5Cardputer.Display.width() - fullWidth) - 5), 5, fullWidth, M5Cardputer.Display.fontHeight());
		M5Cardputer.Display.setTextColor(RED, BLACK);
		M5Cardputer.Display.drawString(batteryStr, ((M5Cardputer.Display.width() - textWidth) - 5), 5);
	}else if (percentage < 75 && percentage > 50) {
		M5Cardputer.Display.fillRect(((M5Cardputer.Display.width() - fullWidth) - 5), 5, fullWidth, M5Cardputer.Display.fontHeight());
		M5Cardputer.Display.setTextColor(YELLOW, BLACK);
		M5Cardputer.Display.drawString(batteryStr, ((M5Cardputer.Display.width() - textWidth) - 5), 5);
	}else {
		M5Cardputer.Display.fillRect(((M5Cardputer.Display.width() - fullWidth) - 5), 5, fullWidth, M5Cardputer.Display.fontHeight());
		M5Cardputer.Display.setTextColor(GREEN, BLACK);
		M5Cardputer.Display.drawString(batteryStr, ((M5Cardputer.Display.width() - textWidth) - 5), 5);
	}
}

void drawTime() {
	struct tm t;
	if (!getLocalTime(&t)) {
		M5Cardputer.Display.drawString("No time", 5, 5);
		return;
	}
	static int lastMinutes = -1;
	int hours = t.tm_hour;
	int minutes = t.tm_min;
	char timeStr[12];
	bool isPM = hours >= 12;
	int displayHour = hours % 12;
	if (displayHour == 0) {
		displayHour = 12;
	}
	snprintf(timeStr, sizeof(timeStr), "%02d:%02d %s", displayHour, minutes, isPM ? "PM" : "AM");
	M5Cardputer.Display.setTextSize(4);
	M5Cardputer.Display.setTextColor(WHITE, BLACK);
	int centerX = ((M5Cardputer.Display.width() / 2) - (M5Cardputer.Display.textWidth(timeStr) / 2));
	int centerY = ((M5Cardputer.Display.height() / 2) - (M5Cardputer.Display.fontHeight() / 2));
	if (firstRun == 0) {
		M5Cardputer.Display.drawString(timeStr, centerX, centerY);
		firstRun = 1;
	}
	M5Cardputer.Display.drawString(timeStr, centerX, centerY);
}

void drawDate() {
	struct tm t;
	if (!getLocalTime(&t)) {
		M5Cardputer.Display.drawString("No date", 5, 5);
		return;
	}
	int year = t.tm_year + 1900;
	int month = t.tm_mon + 1;
	int day = t.tm_mday;
	char dateStr[16];
	snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d", month, day, year);
	M5Cardputer.Display.setTextSize(3);
	M5Cardputer.Display.setTextColor(WHITE, BLACK);
	int centerX = ((M5Cardputer.Display.width() / 2) - (M5Cardputer.Display.textWidth(dateStr) / 2));
	int centerY = ((M5Cardputer.Display.height() / 2) - (M5Cardputer.Display.fontHeight() / 2));
	M5Cardputer.Display.drawString(dateStr, centerX, centerY);
}

void setup() {
	auto cfg = M5.config();
	scroll.begin(&Wire, SCROLL_ADDR, 2, 1, 400000U);
	M5Cardputer.begin(cfg, true);
	M5Cardputer.Display.setRotation(1);
	M5Cardputer.Display.setTextSize(2);
	M5Cardputer.Display.setBrightness(128);
	setSSID();
	setPassword();
	setTimeOffset();
	WiFi.begin(ssid, password);
	int offsetHours = atoi(timeOffset);
	configTime(offsetHours * 3600, 0, "pool.ntp.org", "time.nist.gov");
}

void loop() {
	int16_t delta = scroll.getIncEncoderValue();
	static int screen = 0;
	static int swap = 0;
	screen += delta;
	if (screen > 1) {
		screen = 0;
	}
	if (screen < 0) {
		screen = 1;
	}
	static unsigned long last = 0;
	unsigned long now = millis();
	const unsigned long interval = 5000;
	if (screen == 0) {
		if (swap != 0) {
			swap = 0;
			M5Cardputer.Display.fillScreen(BLACK);
			drawBattery();
		}
		drawTime();
	}else if (screen == 1) {
		if (swap != 1){
			swap = 1;
			M5Cardputer.Display.fillScreen(BLACK);
			drawBattery();
		}
		drawDate();
	}
	if (now - last >= interval) {
		last = now;
		drawBattery();
	}
	delay(20);
}
