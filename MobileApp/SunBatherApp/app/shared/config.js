// file is for urls, apiURl is not tested yet
// 10.0.2.2 is used FOR EMULATOR
// 192.168.0.17 is used for MOBILE PHONE or for talking to another PC/Laptop in emulator
// JACK TODO, work out how to make a config file to change depending on phone or emulator
// JACK TODO make ignore for this file


var guidKey = "268dccb9-8af2-4943-af79-72e168b6af3e";
var localAPIIP = "192.168.0.17";
var localAPIPort = "5001";

module.exports = {
  // Local only example
  apiLocalUrlGet: "http://10.0.2.2:5000/api/TemperatureSensors/1", // find id 1
  apiLocalUrlGetLatest: "http://10.0.2.2:5000/api/TemperatureSensors/GetLatest", // finds latest id (max)
  
  // Mobile phone or local example
  // Get temperature only (example)
  apiGetTemp: "http://" + localAPIIP + ":" + localAPIPort + "/api/TemperatureSensors/1", 
  apiGetTempLatest: "http://" + localAPIIP + ":" + localAPIPort + "/api/TemperatureSensors/GetLatest",

  // Get system status
  apiGetSystemStatus: "http://" + localAPIIP + ":" + localAPIPort + "/api/SystemStatus/" + guidKey,

  // Get record event
  apiGetRecordEvent: "http://" + localAPIIP + ":" + localAPIPort + "/api/RecordEvent/" + guidKey,
  apiGetRecordEventLatest: "http://" + localAPIIP + ":" + localAPIPort + "/api/RecordEvent/GetRecordEventLatest/" + guidKey,
};
