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

function findMin(newMin, originalMin) {
  if (newMin < originalMin) {
    originalMin = newMin;
  } 
  return originalMin;
}

function findMax(newMax, originalMax) {
  if (newMax > originalMax) {
    originalMax = newMax;
  } 
  return originalMax;
}

function loopThroughData(arrayData, initialValue, databaseField, comparitorConstant) {
  var i; 
  var arrayDataInfo = {};
  var min, max, average;
  min = max = average = initialValue;

  for (i = 0; i < arrayData.length; i++){
    arrayData[i].readDateTime = new Date(arrayData[i].readDateTime).getTime();
    min = findMin(arrayData[i][databaseField], min);
    max = findMax(arrayData[i][databaseField], max);
    average = average + arrayData[i][databaseField];

    arrayData[i][databaseField] = arrayData[i][databaseField] * comparitorConstant
  }

  arrayDataInfo.graphDataArray = arrayData;
  arrayDataInfo.min = min;  
  arrayDataInfo.max = max;  
  arrayDataInfo.average = averageRound(average, arrayData.length); 

  return arrayDataInfo;
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
  var graphDataInfo = {};
  graphDataInfo = loopThroughData(graphData, graphData[0][graphOption.databaseField], graphOption.databaseField, 1);

  // store data in array
  graphDataInfo.yAxisInterval = ((graphDataInfo.max - graphDataInfo.min) / 3).toFixed(0);
  graphDataInfo = timePeriod(dateFrom, dateTo, graphDataInfo);

  return graphDataInfo;
}

// export function findGraphDataComparison(dateFrom, dateTo, graphData, graphOption) {
//   var graphDataInfo = {};
//   graphDataInfo = loopThroughData(graphData, graphData[0][graphOption.databaseField], graphOption.databaseField);

//   // store data in array
//   graphDataInfo.yAxisInterval = ((graphDataInfo.max - graphDataInfo.min) / 3).toFixed(0);
//   graphDataInfo = timePeriod(dateFrom, dateTo, graphDataInfo);

//   return graphDataInfo;
// }

export function findGraphDataCompare(graphData, optionChosen) {
  var compareDataInfo = {};
  compareDataInfo = loopThroughData(graphData, graphData[0][optionChosen], optionChosen, 1.5);

  // store data in array and rounds the numbers
  compareDataInfo.min = compareDataInfo.min.toFixed(1); 
  compareDataInfo.max = compareDataInfo.max.toFixed(1);

  console.log(compareDataInfo)
  return compareDataInfo;
}

export function isLiveData(liveData) {
  var dateInfo = {};

   // iso string is good and prevents time zone hassle when converting to C#
  if (liveData) {
    dateInfo.dateNow = new Date().toISOString();
    dateInfo.dateYesterday = new Date(Date.now() - 1 * 24 * 60 * 60 * 1000).toISOString();
    dateInfo.dateNowConvert = new Date(dateInfo.dateNow);
    dateInfo.dateYesterdayConvert = new Date(dateInfo.dateYesterday); 
  } else {
    dateInfo.dateNow = new Date("07/11/2021 12:00").toISOString();
    dateInfo.dateYesterday = new Date("07/10/2021 12:00").toISOString();
    dateInfo.dateNowConvert = new Date(dateInfo.dateNow);
    dateInfo.dateYesterdayConvert = new Date(dateInfo.dateYesterday); 
  }

  return dateInfo;
}