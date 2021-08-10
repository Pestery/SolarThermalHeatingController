import { Http } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

// find min and max of graph (y axis settings)
function graphMinMax(viewModel, min, max) {
    // need to reset otherwise errors
    viewModel.set('graphMin', -5000);
    viewModel.set('graphMax', 5000);

    var diff = (max - min) / 3;

    if (diff < 1) {
        diff = 1;
    }

    viewModel.set('graphMin', (min - diff).toFixed(0));
    viewModel.set('graphMax', (max + diff).toFixed(0));
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
    viewModel.set('axisLegend', 'X axis: Time' + '   ' + 'Y axis: ' + nameOption + "   Red: " + nameCompare + "   White: SunBather");
}

// Used as the arrays spots in the compare.xml (line 20/21)
function graphDataOption(viewModel, databaseField, graphDataArray, dataType) {
    viewModel.set('xAxisData' + dataType, 'readDateTime');
    viewModel.set('yAxisData' + dataType, databaseField);
    viewModel.set('graphData' + dataType, graphDataArray);
}

export function getRecordEventList(dateFrom, dateTo, viewModel, graphOption, graphPage) {
    var dataTypeOption = "Option";
    var dataTypeCompare = "Compare";
    var compare = false; 

    Http.getJSON(apiCall.getRecordEventListToFrom + '/' + dateFrom + '/' + dateTo).then(result => {
        const graphDataInfo = commonFunction.findGraphData(dateFrom, dateTo, result, graphOption);
        //const graphDataInfoComparison = commonFunction.findGraphDataComparison(dateFrom, dateTo, result, graphOption);
        graphDataOption(viewModel, graphOption.databaseField, graphDataInfo.graphDataArray, dataTypeOption);
        graphMinMax(viewModel, graphDataInfo.min, graphDataInfo.max);
        fieldData(viewModel, graphDataInfo.min, graphDataInfo.max, graphDataInfo.average, dataTypeOption);
        graphFormat(viewModel, graphDataInfo.xAxisInterval, graphDataInfo.yAxisInterval, graphDataInfo.xAxisUnit, graphDataInfo.xAxisFormat);
        if (graphPage) {
            graphTitleOption(viewModel, graphOption.name, graphOption.nameAbbreviated);
        }
    }, error => {
        console.log(error);
    });
}

export function getRecordCompareEventList(dateFrom, dateTo, viewModel, graphOption, graphPage) {
    var dataType = "Compare";
    Http.getJSON(apiCall.getRecordEventListToFrom + '/' + dateFrom + '/' + dateTo).then(result => {
        const graphDataInfo = commonFunction.findGraphData(dateFrom, dateTo, result, graphOption);
        graphDataOption(viewModel, graphOption.databaseField, graphDataInfo.graphDataArray, dataType);
        graphMinMax(viewModel, graphDataInfo.min, graphDataInfo.max);
        fieldData(viewModel, graphDataInfo.min, graphDataInfo.max, graphDataInfo.average, dataType);
        graphFormat(viewModel, graphDataInfo.xAxisInterval, graphDataInfo.yAxisInterval, graphDataInfo.xAxisUnit, graphDataInfo.xAxisFormat);
        if (graphPage) {
            graphTitleOption(viewModel, graphOption.name, graphOption.nameAbbreviated);
        }
    }, error => {
        console.log(error);
    });
}

export function getCompetitorEventList(dateFrom, dateTo, viewModel, graphOption, compareOption) {     
    var optionChosen;
    var dataType = "Compare";
    if (graphOption.databaseField == "emissions") {
        optionChosen = compareOption.databaseFieldEmissions;
      } else {
        optionChosen = compareOption.databaseFieldCost;
    }

    Http.getJSON(apiCall.getCompetitorEventListToFrom + '/' + dateFrom + '/' + dateTo).then(result => {
        const compareDataInfo = commonFunction.findGraphDataCompare(result, optionChosen);
        graphDataOption(viewModel, optionChosen, compareDataInfo.graphDataArray, dataType);
        fieldData(viewModel, compareDataInfo.min, compareDataInfo.max, compareDataInfo.average, dataType);
        graphTitleCompare(viewModel, graphOption.name, graphOption.nameAbbreviated, compareOption.name, compareOption.nameAbbreviated);
    }, error => {
        console.log(error);
    });
}