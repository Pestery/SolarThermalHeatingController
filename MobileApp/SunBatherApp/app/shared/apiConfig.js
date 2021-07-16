// file is for urls
// 10.0.2.2 is used FOR EMULATOR
// 192.168.0.17 is used for MOBILE PHONE or for talking to another PC/Laptop in emulator
// JACK TODO, work out how to make a config file to change depending on phone or emulator
// JACK TODO make ignore for this file

var guidKey = "1"; // need to make work for guid, just using int for now for testing -jack
var localAPIIP = "192.168.32.1"; // need input
var localAPIPort = "5001";

module.exports = {
  systemID: "71823731239",
  keyID: 1,
  
  // Local only example
  localUrlGet: "http://10.0.2.2:5000/api/TemperatureSensors/1", // find id 1
  localUrlGetLatest: "http://10.0.2.2:5000/api/TemperatureSensors/GetLatest", // finds latest id (max)
  
  // Mobile phone or local example
  // Get temperature only (example)
  getTemp: "http://" + localAPIIP + ":" + localAPIPort + "/api/TemperatureSensors/1", 
  getTempLatest: "http://" + localAPIIP + ":" + localAPIPort + "/api/TemperatureSensors/GetLatest",

  // Get system status
  getSystemStatus: "http://" + localAPIIP + ":" + localAPIPort + "/api/SystemStatus/" + guidKey,
  putSystemStatus: "http://" + localAPIIP + ":" + localAPIPort + "/api/SystemStatus/" + guidKey,

  // Get record event
  getRecordEventList: "http://" + localAPIIP + ":" + localAPIPort + "/api/RecordEvent/" + guidKey,
  getRecordEventListToFrom: "http://" + localAPIIP + ":" + localAPIPort + "/api/RecordEvent/GetRecordEventDateToFrom/" + guidKey,
  getRecordEventLatest: "http://" + localAPIIP + ":" + localAPIPort + "/api/RecordEvent/GetRecordEventLatest/" + guidKey,
};

