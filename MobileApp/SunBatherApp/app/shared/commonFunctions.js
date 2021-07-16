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

// TO DO, MAKE IT PARSE INTO DATE TIME - JACK
// used by convertDateTime functions, trims datetime
function trimDateTime(readTimeValue) {
  readTimeValue = readTimeValue.slice(-8); // removes date
  readTimeValue = readTimeValue.slice(0,-3); // removes seconds
  return readTimeValue;
}

// finds average and rounds it to 1 decimal place
function averageRound(average, count) {
  average = average / count;
  return average.toFixed(0);
}

// converts datetime to time
export function findGraphData(graphData, graphOption) {
  var i; 
  const graphDataInfo = {};
  var min, max, average;
  min = max = average = graphData[0][graphOption.databaseField];

  //console.log(graphData[0][graphOption.databaseField])
  // trim date to be presentable, find min, max and average total
  for (i = 0; i < graphData.length; i++){
    graphData[i].readDateTime = trimDateTime(graphData[i].readDateTime);

    if (graphData[i][graphOption.databaseField] < min) {
      min = graphData[i][graphOption.databaseField];
    } 

    if (graphData[i][graphOption.databaseField] > max) {
      max = graphData[i][graphOption.databaseField];
    } 

    average = average + graphData[i][graphOption.databaseField];
  }

  // find average
  average = averageRound(average, graphData.length);  

  // store data in array
  graphDataInfo.graphDataArray = graphData;
  graphDataInfo.min = min;
  graphDataInfo.max = max;
  graphDataInfo.average = average;
  graphDataInfo.yAxisInterval = ((max - min) / 3).toFixed(0)

  return graphDataInfo;
}

export function graphOptions() {
  var graphOptionsList = [
    { name: "Temperature Value In", nameAbbreviated: "Temp In", databaseField: "temperatureValueInput", isSelected: false },
    { name: "Temperature Value Out", nameAbbreviated: "Temp Out", databaseField: "temperatureValueOutput", isSelected: false },
    { name: "Temperature Value Roof", nameAbbreviated: "Temp Roof", databaseField: "temperatureValueRoof", isSelected: false },
    { name: "Solar Irradiance", nameAbbreviated: "Solar Energy", databaseField: "solarIrradiance", isSelected: false },
    { name: "Cost", nameAbbreviated: "Cost", databaseField: "cost", isSelected: false },
    { name: "Emissions", nameAbbreviated: "Emissions", databaseField: "emissions", isSelected: false },
    { name: "Energy Adsorbed", nameAbbreviated: "Energy", databaseField: "energyAdsorbed", isSelected: false },
  ];

  return graphOptionsList;
}
