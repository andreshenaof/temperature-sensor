#include <ESP8266WiFi.h>          // Library to connect to Wi-Fi using ESP8266
#include <ESP8266HTTPClient.h>     // Library to handle HTTP requests
#include <ArduinoJson.h>           // Library to parse JSON data from API
#include <NTPClient.h>             // Library for NTP (Network Time Protocol) to get the current time
#include <WiFiUdp.h>               // Library to handle UDP (required for NTP communication)

// WiFi Credentials
const char* ssid     = "";         // Wi-Fi SSID (name of the Wi-Fi network)
const char* password = "";         // Wi-Fi password

// OpenWeatherMap API details
const String apiKey = "";  // Your OpenWeatherMap API key
const String city = "Montreal";  // City to fetch weather data for
const long utcOffsetInSeconds = -18000;  // UTC offset for your time zone (e.g., -18000 for EST)

// NTP Client setup
WiFiUDP ntpUDP;                   // UDP protocol instance for NTP
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds); // Set up NTP client to fetch time

// Weather data from OpenWeatherMap API
WiFiClient client;         // Wi-Fi client for HTTP requests
HTTPClient http;           // HTTP client to handle requests to OpenWeatherMap

void setup() {
  Serial.begin(9600);  // Start serial communication at 9600 baud rate

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);  // Start the connection process with SSID and password
  Serial.print("Connecting to WiFi");
  
  // Wait until the ESP8266 connects to the Wi-Fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);  // Wait for 500 milliseconds before checking the connection status again
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");  // Once connected, print message to the Serial Monitor

  // Initialize NTP Client to get time
  timeClient.begin();  // Start the time client

  // Ensure time is synced with the NTP server
  while (!timeClient.update()) {  // Keep trying to update the time if it's not successful
    Serial.println("Time sync failed, retrying...");  // Print an error message if sync fails
    delay(1000);  // Wait for 1 second before retrying
  }
}

void loop() {
  // Update time from NTP server
  timeClient.update();  // Sync with the NTP server to get the current time
  
  // Request weather data from OpenWeatherMap API
  String weatherUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";  // URL for weather API with the city, API key, and unit set to metric (Celsius)

  http.begin(client, weatherUrl);  // Start an HTTP connection to the OpenWeatherMap API
  int httpCode = http.GET();  // Make the GET request to the weather API and get the HTTP response code
  
  if (httpCode == HTTP_CODE_OK) {  // If the response code is OK (200)
    String payload = http.getString();  // Get the response payload as a string
    //Serial.println("Weather Data: " + payload);  // Optionally, print the raw weather data for debugging

    // Parse JSON data from OpenWeatherMap API
    DynamicJsonDocument doc(1024);  // Create a JSON document to hold the data
    deserializeJson(doc, payload);  // Deserialize the response into the JSON document

    float temperature = doc["main"]["temp"];  // Get the temperature from the JSON data
    float humidity = doc["main"]["humidity"];  // Get the humidity from the JSON data

    // Display weather data
    Serial.print("Temperature: ");
    Serial.print(temperature);  // Print temperature to the serial monitor
    Serial.println(" °C");  // Print unit (Celsius)

    Serial.print("Humidity: ");
    Serial.print(humidity);  // Print humidity to the serial monitor
    Serial.println(" %");  // Print unit (%)
    
  } else {  // If the HTTP request wasn't successful
    Serial.println("Error fetching weather data");  // Print error message
  }

  // End the HTTP connection
  http.end();  // Close the HTTP connection to free up resources

  // Display current time
  Serial.print("Time: ");
  Serial.print(timeClient.getFormattedTime());  // Get and print the formatted time from the NTP client
  Serial.println();  // Print a new line to make the output more readable
  
  // Wait for 10 minutes before the next update
  delay(600000);  // 10 minutes in milliseconds (600,000 ms)
}
