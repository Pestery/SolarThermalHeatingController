#ifndef DATABASE_INTERACTION_H
#define DATABASE_INTERACTION_H

/***** Example POST request *******
POST /echo/post/json HTTP/1.1
Host: website.com
Accept: application/json
Content-Type: application/json
Content-Length: 81

{
	"id":9,
	"value":50.0,
	"readDate":"2005-09-01T13:00:20"
}
***********************/

// Send some data to the database
// A POST request is used as the container
// Returns true on success, or false on failure
bool postToDatabase(const char* databaseAddress, uint16_t databasePort, const String& body) {

	// Make sure the wifi is currently connected
	if (WiFi.status() == WL_CONNECTED) {

		// Use WiFiClient class to create TCP connection
		WiFiClient client;
		if (client.connect(databaseAddress, databasePort)) {

			// Send message to the server
			if (client.connected()) {

				// Send POST request
				client.println(F("POST /api/TemperatureSensors HTTP/1.1"));
				client.println(F("Content-Type: application/json"));
				client.print(F("Content-Length: ")); client.println(body.length());
				client.println();
				client.println(body);
			}

			// Close the connection
			// Return success
			client.stop();
			return true;
		}
	}

	// Return failed to send data
	return false;
}


#endif
