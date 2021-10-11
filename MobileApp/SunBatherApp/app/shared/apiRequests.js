import { Http } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");
var competitorData = require("../shared/competitorData");

// find min and max of graph (y axis settings)
function graphMinMax(viewModel, min, max) {
    // need to reset otherwise errors
    viewModel.set('graphMin', -5000);
    viewModel.set('graphMax', 5000);

    min = Math.floor(min);
    max = Math.floor(max);

    var diff = (max - min) / 3;

    if (diff < 1) { 
        diff = 1;
    }
    viewModel.set('graphMin', (min - diff).toFixed(0));
    viewModel.set('graphMax', (max + diff));
}

// min, max and average of data
function fieldData(viewModel, min, max, average, dataType) {
    viewModel.set('min' + dataType, min);
    viewModel.set('max' + dataType, max);
    viewModel.set('average' + dataType, average);
}

// graph format of page (x axis intervals, date format...)
function graphFormat(viewModel, xAxis, yAxis, xAxisUnit, xAxisFormat) {
    viewModel.set('graphXAxisInterval', xAxis);
    viewModel.set('graphYAxisInterval', yAxis);
    viewModel.set('graphXAxisUnit', xAxisUnit);
    viewModel.set('graphFormat', xAxisFormat);
}

// Title, axis, legend of graph page
function graphTitleOption(viewModel, name, nameAbbreviated) {
    viewModel.set('graphHeader', name);
    viewModel.set('graphSelected', nameAbbreviated);
    viewModel.set('axisLegend', 'X axis: Time' + '             ' + 'Y axis: ' + name);
}

// Title, axis, legend of compare page
function graphTitleCompare(viewModel, nameOption, nameOptionAbbreviated, nameCompare, nameCompareAbbreviated) {
    viewModel.set('graphHeader', nameOption);
    viewModel.set('graphSelected', nameOptionAbbreviated);
    viewModel.set('competitorSelected', nameCompareAbbreviated);
    viewModel.set('axisLegend', 'X axis: Time' + '   ' + 'Y axis: ' + nameOption + "   Red: " + nameCompare + "   Blue: SunBather");
}

// Used as the arrays spots in the compare.xml (line 20/21)
function graphDataOption(viewModel, databaseField, graphDataArray, dataType) {
    viewModel.set('xAxisData' + dataType, 'readDateTime');
    viewModel.set('yAxisData' + dataType, databaseField);
    viewModel.set('graphData' + dataType, graphDataArray);
}

export function getRecordEventList(dateFrom, dateTo, viewModel, graphOption, graphPage, competitor) {
    var dataTypeOption = "Option";
    var dataTypeCompare = "Compare";

    // make sure date from is before date to 
    if (new Date(dateFrom).getTime() >= new Date(dateTo).getTime()) {
        alert("Date From needs to be less than a Date To");
    } else {
        Http.getJSON(apiCall.getRecordEventListToFrom + '/' + dateFrom + '/' + dateTo).then(result => {
            const graphDataInfo = commonFunction.findGraphData(dateFrom, dateTo, result, graphOption);
            graphDataOption(viewModel, graphOption.databaseField, graphDataInfo.graphDataArray, dataTypeOption);

            fieldData(viewModel, graphDataInfo.min, graphDataInfo.max, graphDataInfo.average, dataTypeOption);
            graphFormat(viewModel, graphDataInfo.xAxisInterval, graphDataInfo.yAxisInterval, graphDataInfo.xAxisUnit, graphDataInfo.xAxisFormat);

            if (graphPage) {
                graphTitleOption(viewModel, graphOption.name, graphOption.nameAbbreviated);
                graphMinMax(viewModel, graphDataInfo.min, graphDataInfo.max);
                graphMinMax(viewModel, graphDataInfoComparison.min, graphDataInfoComparison.max);

            } else {
                const graphDataInfoComparison = competitorData.findGraphDataCompare(result, graphOption, competitor);
                graphDataOption(viewModel, graphOption.databaseField, graphDataInfoComparison.graphDataArray, dataTypeCompare);
                graphTitleCompare(viewModel, graphOption.name, graphOption.nameAbbreviated, competitor.name, competitor.nameAbbreviated);
                // ensure min and max of both data sets are correct, otherwise one line may not show up
                var min = commonFunction.findMin(graphDataInfo.min, graphDataInfoComparison.min);
                var max = commonFunction.findMax(graphDataInfo.max, graphDataInfoComparison.max); 
                graphMinMax(viewModel, min, max);
                fieldData(viewModel, graphDataInfoComparison.min, graphDataInfoComparison.max, graphDataInfoComparison.average, dataTypeCompare);
            }
        }, error => {
            console.log(error);
        });
    }
} 

// peforms put requests for the database of updated system status
export function putSystemStatus(updatedPumpMode, updatedManualPumpStatus, updatePumpStatus, updateSetTemperature) {
    var keyID = apiCall.keyID; // this has to be here
    Http.request({
      url: apiCall.putSystemStatus,
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      content: JSON.stringify({
        Id: keyID,
        SystemIdentityID: keyID,
        PumpMode: updatedPumpMode,
        SetTemperature: updateSetTemperature,
        ManualPumpOn: updatedManualPumpStatus,
        PumpStatus: updatePumpStatus,
      }),
    }).then(response => {
      var result = response.content.toJSON();
      console.log(result)
    }, error => { 
        console.error(error);
    });
}

// peforms get requests for the database of current system status
export function getSystemStatus(viewModel) {
    Http.getJSON(apiCall.getSystemStatus).then(result => {
        viewModel.set('pumpStatus', commonFunction.convertOnOff(result.pumpStatus));
        viewModel.set('setTemp', result.setTemperature);       
        viewModel.set('manualPumpStatus', commonFunction.convertOnOff(result.manualPumpOn));
        viewModel.set('pumpMode', commonFunction.convertAutoManual(result.pumpMode));
        viewModel.set('showDetails', !result.pumpMode);
    }, error => {
        console.log(error);
    });
}

// gets the latest recorded temperature
export function getRecordEventLatest(viewModel) {
    Http.getJSON(apiCall.getRecordEventLatest).then(result => {
        viewModel.set('livePoolTemp', commonFunction.addCelcius(result.temperatureValueInput));
    }, error => {
        console.log(error);
    });
}

export function getMainPageData(viewModel) {
    Http.getJSON(apiCall.getSystemStatus).then(result => {
        viewModel.set('pumpStatus', commonFunction.convertOnOff(result.pumpStatus));
        viewModel.set('setTemp', commonFunction.addCelcius(result.setTemperature));         
    }, error => {
        console.log(error);
        
    });

    Http.getJSON(apiCall.getRecordEventLatest).then(result => {
        var temp = result.temperatureValueInput.toFixed(2);
        var roofTemp = result.temperatureValueOutput.toFixed(2); 
        var currentUV = result.solarIrradiance.toFixed(2);
        viewModel.set('poolTemp', commonFunction.addCelcius(temp));
        viewModel.set('roofTemp', commonFunction.addCelcius(roofTemp));
        viewModel.set('currentUV', commonFunction.addUV(currentUV));
    }, error => {
        console.log(error);
    });
}