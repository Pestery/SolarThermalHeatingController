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

// finds average and rounds it to 1 decimal place
function averageRound(average, count) {
  average = average / (count + 1);
  return average.toFixed(1);
}

// if else can be optimized - JACK
function timePeriod(dateFrom, dateTo, graphDataInfo) {
  var dateFromConverted = new Date(dateFrom);
  var dateToConvert = new Date(dateTo);
  var dateDifference = dateToConvert.getDate() - dateFromConverted.getDate();
  var monthDifference = dateToConvert.getMonth() - dateFromConverted.getMonth() + (12 * (dateToConvert.getFullYear() - dateFromConverted.getFullYear()));

  if(dateToConvert.getDate() < dateFromConverted.getDate()){
    monthDifference--;
  }

  if (dateDifference < 0) {
    dateDifference += 30;
  }

  if ((dateDifference <= 1) && (monthDifference <= 0)) {
    graphDataInfo.xAxisInterval = 6;
    graphDataInfo.xAxisUnit = "Hour";
    graphDataInfo.xAxisFormat = "HH:mm";
  } else if ((dateDifference > 1) && (dateDifference <= 3) && (monthDifference <= 0)) {
    graphDataInfo.xAxisInterval = 12;
    graphDataInfo.xAxisUnit = "Hour";
    graphDataInfo.xAxisFormat = "dd/MM HH:mm";
  } else if ((dateDifference > 3) && (dateDifference <= 10) && (monthDifference <= 0)){
    graphDataInfo.xAxisInterval = 2;
    graphDataInfo.xAxisUnit = "Day";
    graphDataInfo.xAxisFormat = "dd/MM";
  } else if ((dateDifference > 10) && (monthDifference <= 0)){
    graphDataInfo.xAxisInterval = 5;
    graphDataInfo.xAxisUnit = "Day";
    graphDataInfo.xAxisFormat = "dd/MM";
  } else if (monthDifference == 1) {
    graphDataInfo.xAxisInterval = 6;
    graphDataInfo.xAxisUnit = "Day";
    graphDataInfo.xAxisFormat = "dd/MM";
  } else if (monthDifference > 1) {
    graphDataInfo.xAxisInterval = 15;
    graphDataInfo.xAxisUnit = "Day";
    graphDataInfo.xAxisFormat = "dd/MM";
  } 

  return graphDataInfo;
}

// converts datetime to time
export function findGraphData(dateFrom, dateTo, graphData, graphOption) {
  var i; 
  var graphDataInfo = {};
  var min, max, average;
  min = max = average = graphData[0][graphOption.databaseField];

  // trim date to be presentable, find min, max and average total
  for (i = 0; i < graphData.length; i++){
    graphData[i].readDateTime = new Date(graphData[i].readDateTime).getTime();

    if (graphData[i][graphOption.databaseField] < min) {
      min = graphData[i][graphOption.databaseField];
    } 

    if (graphData[i][graphOption.databaseField] > max) {
      max = graphData[i][graphOption.databaseField];
    } 

    average = average + graphData[i][graphOption.databaseField];
  }

  // store data in array
  graphDataInfo.graphDataArray = graphData;
  graphDataInfo.min = min;
  graphDataInfo.max = max;
  graphDataInfo.average = averageRound(average, graphData.length); 
  graphDataInfo.yAxisInterval = ((max - min) / 3).toFixed(0);
  graphDataInfo = timePeriod(dateFrom, dateTo, graphDataInfo);

  return graphDataInfo;
}

export function graphOptions() {
  var graphOptionsList = [
    { name: "Temperature Value In", nameAbbreviated: "Temp In", databaseField: "temperatureValueInput", isSelected: false, },
    { name: "Temperature Value Out", nameAbbreviated: "Temp Out", databaseField: "temperatureValueOutput", isSelected: false },
    { name: "Temperature Value Roof", nameAbbreviated: "Temp Roof", databaseField: "temperatureValueRoof", isSelected: false },
    { name: "Solar Irradiance", nameAbbreviated: "Solar Energy", databaseField: "solarIrradiance", isSelected: false },
    { name: "Cost", nameAbbreviated: "Cost", databaseField: "cost", isSelected: false },
    { name: "Emissions", nameAbbreviated: "Emissions", databaseField: "emissions", isSelected: false },
    { name: "Energy Adsorbed", nameAbbreviated: "Energy", databaseField: "energyAdsorbed", isSelected: false },
  ];

  return graphOptionsList;
}

export function graphOptionsCompare() {
  var graphOptionsCompareList = [
    { name: "Cost", nameAbbreviated: "Cost", databaseField: "cost", isSelected: false },
    { name: "Emissions", nameAbbreviated: "Emissions", databaseField: "emissions", isSelected: false },
  ];

  return graphOptionsCompareList;
}

export function competitorOptions() {
  var competitorOptionsList = [
    { name: "Gas Heater", nameAbbreviated: "Heater", databaseFieldCost: "gasPumpCost", databaseFieldEmissions: "gasPumpEmissions", isSelected: false },
    { name: "Gas Pump", nameAbbreviated: "Pump", databaseFieldCost: "gasHeaterCost", databaseFieldEmissions: "gasHeaterEmissions", isSelected: false },
  ];

  return competitorOptionsList;
}

export function createGraphData(graphData, optionChosen) {
  var i; 
  var compareDataInfo = {};
  var min, max, average;

  min = max = average = graphData[0][optionChosen];

  // trim date to be presentable, find min, max and average total
  for (i = 0; i < graphData.length; i++){
    graphData[i].readDateTime = new Date(graphData[i].readDateTime).getTime();

    if (graphData[i][optionChosen] < min) {
      min = graphData[i][optionChosen];
    } 

    if (graphData[i][optionChosen] > max) {
      max = graphData[i][optionChosen];
    } 

    average = average + graphData[i][optionChosen];
  }

  // store data in array
  compareDataInfo.graphDataArray = graphData;
  compareDataInfo.min = min.toFixed(1);
  compareDataInfo.max = max.toFixed(1);
  compareDataInfo.average = averageRound(average, graphData.length);

  return compareDataInfo;
}