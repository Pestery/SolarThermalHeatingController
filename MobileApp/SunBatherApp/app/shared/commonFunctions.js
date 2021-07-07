// File contains all common functions used by most classes

// converts string to bool
export function convertToBool(message) {
    if (message.toLowerCase() == "auto" || message.toLowerCase() == "on") {
      return true;
    }
    return false;
}

// convers true/false to on/off
export function convertOnOff(message) {
    var sentence;
    if (message) {
      sentence = "On";
    } else {
      sentence = "Off";
    }
    return sentence;
}

export function convertAutoManual(message) {
    var sentence;
    if (message) {
      sentence = "Auto"; 
    } else {
      sentence = "Manual";
    }
    return sentence;
}

// adds celcius to the value
export function addCelcius(message){
    var sentence;
    sentence = message + " C"
    return sentence;
}
  
// adds UV to the value
export function addUV(message){
    var sentence;
    sentence = message + " W/M^2"
    return sentence;
}

// converts datetime to time
export function convertDateTime(graphData){
    var i;
    var convertDateTime;

    for (i = 0; i < graphData.length; i++){
        convertDateTime = graphData[i].readDateTime;
        convertDateTime = convertDateTime.slice(-8); // removes date
        convertDateTime = convertDateTime.slice(0,-3); // removes seconds
        graphData[i].readDateTime = convertDateTime;
    }
    return graphData;
}