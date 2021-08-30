var commonFunction = require("../shared/commonFunctions");

//assumptions
var timeInterval = 15; // Default sensor reading is 15 min

//variables
var naturalGasCostpkW = 0.04;   //default cost of 4c per kW
var electricityCostpkW = 0.4;   //default cost of 40pc per kW

//constants
const MASSFLOWRATE = 1.7448; //kg/s
const WATERHEATCAPACITY = 4.184; //kJ/kg.K
const GASEMISSION = 8.31 * timeInterval; // default 499 CO2e/kWH
const COALEMISSION = 14.8 * timeInterval; //defualt is 888 measurement in CO2e/kWh

//COP's for the 4 kinds of heaters
const ELECTRICHEATER1 = 5.58 //RTHP026-1
const ELECTRICHEATER2 = 5.39 //RTHP024-1
const GASHEATER1 = 0.79 //P0127
const GASHEATER2 = 0.75 //127 Premium

function getThermalPower(data) {
    var inletTemp = data.temperatureValueInput; //Degrees Celcius
    var outletTemp = data.temperatureValueOutput; //Degrees Celcius
    var deltaTemp = outletTemp - inletTemp; //Degrees Celcius
    return deltaTemp*WATERHEATCAPACITY*MASSFLOWRATE; //KW
}

function loopThroughData(arrayData, databaseField, energyConstant, heaterConstant) {
    var i; 
    var arrayDataInfo = {};
    var min, max, average;
    var thermalPowerAbsorbed;
    var test = getThermalPower(arrayData[0]);
    min = max = average = 0;
    console.log(arrayData[0][databaseField])
    console.log(test)
    console.log(energyConstant)
    console.log(heaterConstant)

    for (i = 0; i < arrayData.length; i++){
        arrayData[i].readDateTime = new Date(arrayData[i].readDateTime).getTime();
        thermalPowerAbsorbed = getThermalPower(arrayData[i]);
        arrayData[i][databaseField] = (thermalPowerAbsorbed*energyConstant)/heaterConstant;
        console.log(arrayData[i][databaseField])
        min = commonFunction.findMin(arrayData[i][databaseField], min);
        max = commonFunction.findMax(arrayData[i][databaseField], max);
        average = average + arrayData[i][databaseField];
    }

    console.log('finished loop')
    arrayDataInfo.graphDataArray = arrayData;
    arrayDataInfo.min = min;  
    arrayDataInfo.max = max;  
    arrayDataInfo.average = commonFunction.averageRound(average, arrayData.length); 
    
    return arrayDataInfo;
}

export function findGraphDataCompare(graphData, graphOption, optionChosen) {
    var compareDataInfo = {};
    console.log('test')
    console.log(graphOption)
    console.log(optionChosen)

    switch (optionChosen.idName) {
        case "ELECTRICHEATER1":
                if (graphOption.name == "Cost") {
                    compareDataInfo = loopThroughData(graphData, graphOption.databaseField, electricityCostpkW, ELECTRICHEATER1);
                } else {
                    compareDataInfo = loopThroughData(graphData, graphOption.databaseField, COALEMISSION, ELECTRICHEATER1);
                }
            break;
        case "ELECTRICHEATER2":
                if (graphOption.name == "Cost") {
                    compareDataInfo = loopThroughData(graphData, graphOption.databaseField, electricityCostpkW, ELECTRICHEATER2);
                } else {
                    compareDataInfo = loopThroughData(graphData, graphOption.databaseField, COALEMISSION, ELECTRICHEATER2);
                }
            break;
        case "GASHEATER1":
                if (graphOption.name == "Cost") {
                    compareDataInfo = loopThroughData(graphData, graphOption.databaseField, naturalGasCostpkW, GASHEATER1);
                } else {
                    compareDataInfo = loopThroughData(graphData, graphOption.databaseField, GASEMISSION, GASHEATER1);
                }
            break;
        default:
                if (graphOption.name == "Cost") {
                    compareDataInfo = loopThroughData(graphData, graphOption.databaseField, naturalGasCostpkW, GASHEATER2);
                } else {
                    compareDataInfo = loopThroughData(graphData, graphOption.databaseField, GASEMISSION, GASHEATER2);
                }
    }

    console.log('complete')
    // store data in array and rounds the numbers
    compareDataInfo.yAxisInterval = ((compareDataInfo.max - compareDataInfo.min) / 3).toFixed(0);
    compareDataInfo.min = compareDataInfo.min.toFixed(1); 
    compareDataInfo.max = compareDataInfo.max.toFixed(1);
  
    return compareDataInfo;
  }