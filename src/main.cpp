#include <Arduino.h>
#include <M5Cardputer.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

int firstRun = 0;
String ssid = "";
String password = "";
String timeOffset = "";

void setSSID() {
	ssid = "";
	bool done = false;
	while (!done) {
		M5Cardputer.update();
		M5Cardputer.Display.setCursor(0, 0);
		M5Cardputer.Display.println("Enter SSID: ");
		auto status = M5Cardputer.Keyboard.keysState();

		for (char i : status.word) {
			if (ssid.length() < 32) {
				ssid += i;
			}
		}
		if (status.del && ssid.length() >= 0) {
			if (status.del && ssid.length() > 0) {
				ssid.remove(ssid.length() - 1);
			}
			M5Cardputer.Display.fillRect(0, 20, 240, 115);
		}
		if (status.enter) {
			done = true;
		}
		M5Cardputer.Display.setCursor(0, 20);
		M5Cardputer.Display.println(ssid);
		delay(150);
	}
	M5Cardputer.Display.fillScreen(BLACK);
	M5Cardputer.Display.setCursor(0, 0);
	M5Cardputer.Display.println("SSID set to:");
	M5Cardputer.Display.println(ssid);
	delay(2000);
	M5Cardputer.Display.fillScreen(BLACK);
}

void setPassword() {
	password = "";
	bool done = false;
	while (!done) {
		M5Cardputer.update();
		M5Cardputer.Display.setCursor(0, 0);
		M5Cardputer.Display.println("Enter password:");
		auto status = M5Cardputer.Keyboard.keysState();

		for (char i : status.word) {
			if (password.length() < 20) {
				password += i;
			}
		}
		if (status.del && password.length() >= 0) {
			if (status.del && password.length() > 0) {
				password.remove(password.length() - 1);
			}
			M5Cardputer.Display.fillRect(0, 20, 240, 115);
		}
		if (status.enter) {
			done = true;
		}
		M5Cardputer.Display.setCursor(0, 20);
		M5Cardputer.Display.println(password);
		delay(150);
	}
	M5Cardputer.Display.fillScreen(BLACK);
	M5Cardputer.Display.setCursor(0, 0);
	M5Cardputer.Display.println("Password set to:");
	M5Cardputer.Display.println(password);
	delay(2000);
	M5Cardputer.Display.fillScreen(BLACK);
}

void setTimeOffset() {
	timeOffset = "";
	bool done = false;
	while (!done) {
		M5Cardputer.update();
		M5Cardputer.Display.setCursor(0, 0);
		M5Cardputer.Display.println("Enter UTC Offset:");
		auto status = M5Cardputer.Keyboard.keysState();

		for (char i : status.word) {
			if ((i >= '0' && i <= '9') || (i == '-' && timeOffset.length() == 0)) {
				timeOffset += i;
			}
		}
		if (status.del && timeOffset.length() >= 0) {
			if (status.del && timeOffset.length() > 0) {
				timeOffset.remove(timeOffset.length() - 1);
			}
			M5Cardputer.Display.fillRect(0, 20, 240, 115);
		}
		if (status.enter) {
			if (timeOffset.toInt() <= 14 && timeOffset.toInt() >= -12) {
				done = true;
			}else {
				M5Cardputer.Display.fillScreen(BLACK);
				timeOffset = "";
				M5Cardputer.Display.drawString("Invalid UTC offset", 0, 0);
				delay(2000);
				M5Cardputer.Display.fillScreen(BLACK);
			}
		}
		M5Cardputer.Display.setCursor(0, 20);
		M5Cardputer.Display.println(timeOffset);
		delay(150);
	}
	if (timeOffset.toInt() <= 14 && timeOffset.toInt() >= -12) {
		M5Cardputer.Display.fillScreen(BLACK);
		M5Cardputer.Display.setCursor(0, 0);
		M5Cardputer.Display.println("Timezone set to:");
		M5Cardputer.Display.println(timeOffset);
		delay(2000);
		M5Cardputer.Display.fillScreen(BLACK);
	}else {
		M5Cardputer.Display.fillScreen(BLACK);
	}
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
	timeClient.update();
	static int lastMinutes = -1;
	int hours = timeClient.getHours();
	int minutes = timeClient.getMinutes();
	char timeStr[12];
	if (hours == 12) {
		snprintf(timeStr, sizeof(timeStr), "%02d:%02d PM", hours, minutes);
	}else if (hours == 0) {
		snprintf(timeStr, sizeof(timeStr), "%02d:%02d AM", (hours + 12), minutes);
	}else {
		if (hours < 12) {
			snprintf(timeStr, sizeof(timeStr), "%02d:%02d AM", hours, minutes);
		}else {
			snprintf(timeStr,sizeof(timeStr), "%02d:%02d PM", (hours - 12), minutes);
		}
	}
	M5Cardputer.Display.setTextSize(4);
	M5Cardputer.Display.setTextColor(WHITE, BLACK);
	int centerX = ((M5Cardputer.Display.width() / 2) - (M5Cardputer.Display.textWidth(timeStr) / 2));
	int centerY = ((M5Cardputer.Display.height() / 2) - (M5Cardputer.Display.fontHeight() / 2));
	if (firstRun == 0) {
		M5Cardputer.Display.drawString(timeStr, centerX, centerY);
		firstRun = 1;
	}
	if (minutes != lastMinutes) {
		int newHours = timeClient.getHours();
		int newMinutes = timeClient.getMinutes();
		lastMinutes = newMinutes;
		char newTimeStr[12];
		if (newHours == 12) {
			snprintf(newTimeStr, sizeof(newTimeStr), "%02d:%02d PM", newHours, newMinutes);
		}else if (newHours == 0) {
			snprintf(newTimeStr, sizeof(newTimeStr), "%02d:%02d AM", (newHours + 12), newMinutes);
		}else {
			if (newHours < 12) {
				snprintf(newTimeStr, sizeof(newTimeStr), "%02d:%02d AM", newHours, newMinutes);
			}else {
				snprintf(newTimeStr, sizeof(newTimeStr), "%02d:%02d PM", (newHours - 12), newMinutes);
			}
		}
		M5Cardputer.Display.drawString(newTimeStr, centerX, centerY);
	}
}

void setup() {
	auto cfg = M5.config();
	M5Cardputer.begin(cfg, true);
	M5Cardputer.Display.setRotation(1);
	M5Cardputer.Display.setTextSize(2);
	M5Cardputer.Display.setBrightness(128);
	setSSID();
	setPassword();
	setTimeOffset();
	WiFi.begin(ssid, password);
	timeClient.begin();
	int offsetHours = timeOffset.toInt();
	timeClient.setTimeOffset(offsetHours * 3600);
}

void loop() {
	unsigned long last = 0;
	unsigned long now = millis();
	const unsigned long interval = 5000;

	drawTime();
	if (now - last >= interval) { 
		drawBattery();
	}
	delay(1000);
}
