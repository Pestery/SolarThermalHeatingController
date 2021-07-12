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
export function addCelcius(message) {
  var sentence;
  sentence = message + " C"
  return sentence;
}
  
// adds UV to the value
export function addUV(message) {
  var sentence;
  sentence = message + " W/M^2"
  return sentence;
}

// used by convertDateTime functions, trims datetime
function trimDateTime(readTimeValue) {
  readTimeValue = readTimeValue.slice(-8); // removes date
  readTimeValue = readTimeValue.slice(0,-3); // removes seconds
  return readTimeValue;
}

// converts datetime to time
export function findGraphData(graphData) {
  var i; 
  const graphDataInfo = {};
  var min = graphData[0].temperatureValueRoof;
  var max = graphData[0].temperatureValueRoof;
  var average = graphData[0].temperatureValueRoof;

  for (i = 0; i < graphData.length; i++){
    graphData[i].readDateTime = trimDateTime(graphData[i].readDateTime);

    if (graphData[i].temperatureValueRoof < min) {
      min = graphData[i].temperatureValueRoof;
    } 

    if (graphData[i].temperatureValueRoof > max) {
      max = graphData[i].temperatureValueRoof;
    } 

    average = average + graphData[i].temperatureValueRoof;
  }

  average = average / graphData.length;

  graphDataInfo.graphDataArray = graphData;
  graphDataInfo.min = min;
  graphDataInfo.max = max;
  graphDataInfo.average = average;

  return graphDataInfo;
}

