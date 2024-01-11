/* Copyright 2023 Dual Tachyon
 * https://github.com/DualTachyon
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#include <string.h>

#include "driver/eeprom.h"
#include "driver/keyboard.h"
#include "driver/st7565.h"
#include "driver/system.h"
#include "external/printf/printf.h"
#include "helper/battery.h"
#include "app/app.h"
#include "settings.h"
#include "audio.h"
#include "misc.h"
#include "ui/helper.h"
#include "ui/welcome.h"
#include "ui/status.h"
#include "version.h"

int customRand(int min, int max, int seed) {
    seed = (seed * 333 + 12345) & 0x7fffffff;  // Linear congruential generator
    return min + (seed % (max - min + 1));
}

void UI_DisplayReleaseKeys(void)
{
	memset(gStatusLine,  0, sizeof(gStatusLine));
	UI_DisplayClear();

	UI_PrintString("RELEASE", 0, 127, 1, 10);
	UI_PrintString("ALL KEYS", 0, 127, 3, 10);

	ST7565_BlitStatusLine();  // blank status line
	ST7565_BlitFullScreen();
}

void UI_DisplayWelcome(void)
{
	char WelcomeString0[16];
	char WelcomeString1[16];

	memset(gStatusLine,  0, sizeof(gStatusLine));
	BACKLIGHT_TurnOn();
	UI_DisplayClear();

	if (gEeprom.POWER_ON_DISPLAY_MODE == POWER_ON_DISPLAY_MODE_NONE) {
		ST7565_FillScreen(0xFF);
	} else {
		memset(WelcomeString0, 0, sizeof(WelcomeString0));
		memset(WelcomeString1, 0, sizeof(WelcomeString1));

		if (gEeprom.POWER_ON_DISPLAY_MODE == POWER_ON_DISPLAY_MODE_FULL_SCREEN) {
			int ButtonCount = 1;
			ST7565_FillScreen(0xFF);
			AUDIO_PlayBeep(BEEP_440HZ_500MS);
			UI_DisplayClear();
			SYSTEM_DelayMs(1000);			
			if (KEYBOARD_Poll() != KEY_MENU) {
				while (true) {
					BACKLIGHT_TurnOff();
					UI_DisplayClear();
					SYSTEM_DelayMs(300);
					BACKLIGHT_TurnOn();
					AUDIO_PlayBeep(BEEP_880HZ_60MS_TRIPLE_BEEP);
					if (KEYBOARD_Poll() != KEY_INVALID) {
						ButtonCount++;
						if (ButtonCount < 15) {
							UI_PrintString("STOP!", 0, 127, 2, 10);
						} else if (ButtonCount < 30) {
							UI_PrintString("RELEASE", 0, 127, 1, 10);
							UI_PrintString("ALL KEYS", 0, 127, 3, 10);
						} else if (ButtonCount < 50) {
							UI_PrintString("RELEASE", 0, 127, 1, 10);
							UI_PrintString("THEM NOW", 0, 127, 3, 10);
						} else if (ButtonCount < 80) {
							UI_PrintString("FUCK OFF", 0, 127, 1, 10);
							UI_PrintString("DOG", 0, 127, 3, 10);
						} else {
							UI_PrintString("STILL", 0, 127, 1, 10);
							UI_PrintString("UNAUTHORISED", 0, 127, 3, 10);
							UI_PrintString("CUNT", 0, 127, 5, 10);
						}
					} else {
						//ButtonCount--;
						UI_PrintString("UNAUTHORISED", 0, 127, 2, 10);
					}
					ST7565_BlitStatusLine();
					ST7565_BlitFullScreen();
				}
			}

			UI_PrintString("Welcome", 0, 127, 2, 10);
			ST7565_BlitStatusLine();
			ST7565_BlitFullScreen();
			SYSTEM_DelayMs(500);
			AUDIO_PlayBeep(BEEP_500HZ_60MS_DOUBLE_BEEP);
			AUDIO_PlayBeep(BEEP_440HZ_40MS_OPTIONAL);
			AUDIO_PlayBeep(BEEP_500HZ_60MS_DOUBLE_BEEP);
			AUDIO_PlayBeep(BEEP_880HZ_200MS);
		} else { 
			if (gEeprom.POWER_ON_DISPLAY_MODE == POWER_ON_DISPLAY_MODE_VOLTAGE)
			{
				strcpy(WelcomeString0, "VOLTAGE");
				sprintf(WelcomeString1, "%u.%02uV %u%%",
					gBatteryVoltageAverage / 100,
					gBatteryVoltageAverage % 100,
					BATTERY_VoltsToPercent(gBatteryVoltageAverage));
			}
			else
			{
				EEPROM_ReadBuffer(0x0EB0, WelcomeString0, 16);
				EEPROM_ReadBuffer(0x0EC0, WelcomeString1, 16);
			}

			UI_PrintString(WelcomeString0, 0, 127, 0, 10);
			UI_PrintString(WelcomeString1, 0, 127, 2, 10);
			UI_PrintStringSmallNormal(Version, 0, 128, 6);
		}

		ST7565_BlitStatusLine();  // blank status line
		ST7565_BlitFullScreen();
	}
}
