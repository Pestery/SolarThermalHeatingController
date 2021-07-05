// file is for urls
// 10.0.2.2 is used FOR EMULATOR
// 192.168.0.17 is used for MOBILE PHONE or for talking to another PC/Laptop in emulator
// JACK TODO, work out how to make a config file to change depending on phone or emulator
// JACK TODO make ignore for this file

var guidKey = "1"; // need to make work for guid, just using int for now for testing -jack
var localAPIIP = "192.168.32.1"; // need input
var localAPIPort = "5001";

module.exports = {
  SystemID: "71823731239",

  // Local only example
  LocalUrlGet: "http://10.0.2.2:5000/api/TemperatureSensors/1", // find id 1
  LocalUrlGetLatest: "http://10.0.2.2:5000/api/TemperatureSensors/GetLatest", // finds latest id (max)
  
  // Mobile phone or local example
  // Get temperature only (example)
  GetTemp: "http://" + localAPIIP + ":" + localAPIPort + "/api/TemperatureSensors/1", 
  GetTempLatest: "http://" + localAPIIP + ":" + localAPIPort + "/api/TemperatureSensors/GetLatest",

  // Get system status
  GetSystemStatus: "http://" + localAPIIP + ":" + localAPIPort + "/api/SystemStatus/" + guidKey,

  // Get record event
  GetRecordEvent: "http://" + localAPIIP + ":" + localAPIPort + "/api/RecordEvent/" + guidKey,
  GetRecordEventLatest: "http://" + localAPIIP + ":" + localAPIPort + "/api/RecordEvent/GetRecordEventLatest/" + guidKey,
};
