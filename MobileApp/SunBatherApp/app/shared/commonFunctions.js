// File contains all common functions used by most classes

// convers true/false to on/off
export function convertBool(message) {
    var sentence;
    if (message == true){
      sentence = "On";
    } else{
      sentence = "Off";
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