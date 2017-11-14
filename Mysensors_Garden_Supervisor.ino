/**
 * MySensors_Garden_Supervisor
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0: GUILLOTON Laurent
 * Version 1.1 - 2017-07-27: Cr�ation du sketch initial
 * Version 1.2 - 2017-11-07: Modification du sketch
 *
 * DESCRIPTION
 *
 *
 * Sketch g�rant la mesure de la temp�rature et de l'humidit� de l'air, de l'humidit� de sol, et de la luminosit� ambiante
 * et le niveau d'eau et qui pilote une pompe et un �clairage de croissance et un ventilateur.
 * Toutes les Datas sont exploitables via mysensors
 *
 */

/* Activer les logs de debug dans le moniteur s�rie */
#define MY_DEBUG

/* S�lection du module radio */
#define MY_RADIO_NRF24

/* Activation de la fonction r�p�teur (car aliment�) */
#define MY_REPEATER_FEATURE

/* D�claration des librairies */

#include "Arduino.h"
#include <SPI.h>
#include <MySensors.h>
#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>
#include <SHT31.h>

/* C�blage */
#define I2C_ADDRESS_OLED 0x3C	// Adresse I2C de l'�cran OLED
SSD1306AsciiWire oled;			// Instantiation de l'�cran OLED

SHT31 sht31 = SHT31();

#define SPRINKLER_ACTUATOR_PIN 5		// Interruption
#define LIGHT_ACTUATOR_PIN 6 	// Button start/select
#define LIGHT_SENSOR_ANALOG_PIN 0
#define SOIL_SENSOR_PIN 1
#define FAN_ACTUATOR_PIN 7
#define LEVEL_SENSOR_PIN 8



/* Variables globales */
#define RELAY_ON 1  // GPIO value to write to turn on attached relay
#define RELAY_OFF 0 // GPIO value to write to turn off attached relay


// initialisation des constantes
unsigned long SLEEP_TIME = 60000; // Sleep time between reads (in milliseconds)
unsigned long lastRefreshTime = 0; // Use this to implement a non-blocking delay function

float Temperature;
float Humidity;
float Moisture;
float LightLevel;
bool LevelValue;

float lastTemp;
float lastHum;
float lastMois;
float lastLightLevel;
bool lastLevelValue;

/* Configuration Mysensors */
#define CHILD_ID_TEMP 1
#define CHILD_ID_HUM 2
#define CHILD_ID_SOIL 3
#define CHILD_ID_LIGHT_LEVEL 4
#define CHILD_ID_LEVEL 5
#define CHILD_ID_SPRINKLER 6
#define CHILD_ID_LIGHT 7
#define CHILD_ID_FAN 8


MyMessage msgTemp(CHILD_ID_TEMP, V_TEMP);
MyMessage msgHum(CHILD_ID_HUM, V_HUM);
MyMessage msgSoil(CHILD_ID_SOIL, V_LEVEL);
MyMessage msgLightLevel(CHILD_ID_LIGHT_LEVEL, V_LIGHT_LEVEL);
MyMessage msgLevel(CHILD_ID_LEVEL, V_TRIPPED);
MyMessage msgSprinkler(CHILD_ID_SPRINKLER, V_STATUS);
MyMessage msgLight(CHILD_ID_LIGHT, V_STATUS);
MyMessage msgFan(CHILD_ID_FAN, V_STATUS);

void updateDisplay();
void algorithme();

/* Fonction de pr�sentation du noeud */
void presentation()  {
	// Envoi du nom et de la version du sketch
	sendSketchInfo("Mysensors_Garden_Supervisor", "1.2");
	// Envoi de la pr�sentation des capteurs associ�s au noeud
	present(CHILD_ID_TEMP, S_TEMP);
	present(CHILD_ID_HUM, S_HUM);
	present(CHILD_ID_SOIL, S_MOISTURE);
	present(CHILD_ID_LIGHT_LEVEL, S_LIGHT_LEVEL);
	present(CHILD_ID_LEVEL, S_DOOR);
	present(CHILD_ID_SPRINKLER, S_BINARY);
	present(CHILD_ID_LIGHT, S_BINARY);
	present(CHILD_ID_FAN, S_BINARY);

}

//The setup function is called once at startup of the sketch
void setup()
{
	Serial.begin(115200);
	Wire.begin();
	sht31.begin();
	oled.begin(&Adafruit128x64, I2C_ADDRESS_OLED);
	oled.set400kHz();
	oled.setFont(Adafruit5x7);

	// R�cup�ration des donn�es en m�moire dans l'EEPROM
	pinMode(SPRINKLER_ACTUATOR_PIN, OUTPUT);
	digitalWrite(SPRINKLER_ACTUATOR_PIN, loadState(CHILD_ID_SPRINKLER));
	send(msgSprinkler.set(loadState(CHILD_ID_SPRINKLER)));

	pinMode(LIGHT_ACTUATOR_PIN, OUTPUT);
	digitalWrite(LIGHT_ACTUATOR_PIN, loadState(CHILD_ID_LIGHT));
	send(msgLight.set(loadState(CHILD_ID_LIGHT)));

	pinMode(FAN_ACTUATOR_PIN, OUTPUT);
	digitalWrite(FAN_ACTUATOR_PIN, loadState(CHILD_ID_FAN));
	send(msgFan.set(loadState(CHILD_ID_FAN)));
}

// The loop function is called in an endless loop
void loop()
{
	boolean needRefresh = (millis() - lastRefreshTime) > SLEEP_TIME;
	if (needRefresh)
	{
		lastRefreshTime = millis();
		Temperature = sht31.getTemperature();
		if (isnan(Temperature)) {
			Serial.println("Failed reading temperature from sensor");
		} else if (Temperature != lastTemp) {
			lastTemp = Temperature;
			send(msgTemp.set(Temperature, 1));
		}
		Humidity = sht31.getHumidity();
		if (isnan(Humidity)) {
			Serial.println("Failed reading humidity from sensor");
		} else if (Humidity != lastHum) {
			lastHum = Humidity;
			send(msgHum.set(Humidity, 1));
		}
		Moisture = analogRead(SOIL_SENSOR_PIN);
		if (isnan(Moisture)) {
			Serial.println("Failed reading soil moisture from sensor");
		} else if (Moisture != lastMois) {
			lastMois = Moisture;
			send(msgSoil.set(Moisture, 1));
		}
		LightLevel = (1023-analogRead(LIGHT_SENSOR_ANALOG_PIN))/10.23;
		if (isnan(LightLevel)) {
			Serial.println("Failed reading light level from sensor");
		} else if (LightLevel != lastLightLevel) {
			lastLightLevel = LightLevel;
			send(msgLightLevel.set(LightLevel, 1));
		}
	}
	LevelValue = digitalRead(LEVEL_SENSOR_PIN);
	if (isnan(LevelValue)) {
		Serial.println("Failed reading level value from sensor");
	} else if (LevelValue != lastLevelValue) {
		lastLevelValue = LevelValue;
		send(msgLevel.set(LevelValue, 1));
	}

	//Mise � jour de l'affichage en temps r�el
	updateDisplay();

	// lance la fonction g�rant l'automatisation de l'arrosage
	algorithme();
}

void receive(const MyMessage &message)
{
	// We only expect one type of message from controller. But we better check anyway.
	if (message.type==V_STATUS)
	{
		bool state = message.getBool();
		if(message.sensor==CHILD_ID_SPRINKLER)
		{
			digitalWrite(SPRINKLER_ACTUATOR_PIN, state);
			saveState(CHILD_ID_SPRINKLER, state);
			send(msgSprinkler.set(state));
		}
		if(message.sensor==CHILD_ID_LIGHT)
		{
			digitalWrite(LIGHT_ACTUATOR_PIN, state);
			saveState(CHILD_ID_LIGHT, state);
			send(msgLight.set(state));
		}
		if(message.sensor==CHILD_ID_FAN)
		{
			digitalWrite(FAN_ACTUATOR_PIN, state);
			saveState(CHILD_ID_FAN, state);
			send(msgFan.set(state));
		}
	}
}

void updateDisplay()
{
	oled.set1X();
	oled.setCursor ( 0, 1 );
	// Affichage Temp�rature
	oled.print("T=");
	oled.print(Temperature);
	oled.println("C");
	// Affichage humidit� ambiante
	oled.print("H=");
	oled.print(Humidity);
	oled.println("%");
	// Affichage humidit� de sol
	oled.print("S=");
	oled.println(Moisture);
	// Affichage luminosit� ambiante
	oled.print("L=");
	oled.println(LightLevel);
}

void algorithme()
{
	/*if ((Moisture<100) && (loadState(CHILD_ID_SPRINKLER)==0))
	{
		digitalWrite(SPRINKLER_ACTUATOR_PIN, RELAY_ON);
		saveState(CHILD_ID_SPRINKLER, RELAY_ON);
		send(msgSprinkler.set(RELAY_ON));
	}
	else if ((Moisture>150) &&  (loadState(CHILD_ID_SPRINKLER)==1))
	{
		digitalWrite(SPRINKLER_ACTUATOR_PIN, RELAY_OFF);
		saveState(CHILD_ID_SPRINKLER, RELAY_OFF);
		send(msgSprinkler.set(RELAY_OFF));
	}*/
}
