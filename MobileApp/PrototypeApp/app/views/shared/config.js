// file is for urls, apiURl is not tested yet
// 10.0.2.2 is used FOR EMULATOR
// 192.168.0.17 is used for MOBILE PHONE or for talking to another PC/Laptop in emulator
// JACK TODO, work out how to make a config file to change depending on phone or emulator
module.exports = {
    apiLocalUrlGet: "http://10.0.2.2:5000/api/TemperatureSensors/1", // find id 1
    apiLocalUrlGetLatest: "http://10.0.2.2:5000/api/TemperatureSensors/GetLatest", // finds latest id (max)
    apiMobileUrlGet: "http://192.168.68.110:5001/api/TemperatureSensors/1", 
    apiMobileUrlGetLatest: "http://192.168.68.110:5001/api/TemperatureSensors/GetLatest",
  };