// file is for urls, apiURl is not tested yet
// 10.0.2.2 is used FOR EMULATOR
// 192.168.0.17 is used for MOBILE PHONE or for talking to another PC/Laptop in emulator
// JACK TODO, work out how to make a config file to change depending on phone or emulator

var guidKey = "48b25e0d-a114-4bc5-b707-72ff67a98ce7";

module.exports = {
  // Local only example
  apiLocalUrlGet: "http://10.0.2.2:5000/api/TemperatureSensors/1", // find id 1
  apiLocalUrlGetLatest: "http://10.0.2.2:5000/api/TemperatureSensors/GetLatest", // finds latest id (max)
  
  // Mobile phone or local example
  // Get temperature only (example)
  apiGetTemp: "http://192.168.68.110:5001/api/TemperatureSensors/1", 
  apiGetTempLatest: "http://192.168.68.110:5001/api/TemperatureSensors/GetLatest",

  // Get system status
  apiGetSystemStatus: "http://192.168.68.110:5001/api/SystemStatus/" + guidKey,

  // Get record event
  apiGetRecordEvent: "http://192.168.68.110:5001/api/RecordEvent/" + guidKey,
  apiGetRecordEventLatest: "http://192.168.68.110:5001/api/RecordEvent/GetRecordEventLatest/" + guidKey,
};
