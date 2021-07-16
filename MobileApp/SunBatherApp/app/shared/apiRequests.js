import { Http } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

// required for changing tables, when graphMin is > than graphMax, its bugs out. This prevents that - jack
function resetMinMax() {
    viewModel.set('graphMin', -5000);
    viewModel.set('graphMax', 5000);
}

// works for now, will need to be cleaned up 
export function getRecordEventList(dateFrom, dateTo, viewModel, graphOption) {
    // console.log(Object.keys(result[0])) can be used to find array property names, could be useful in future for decoupling option data from database - JACK
    Http.getJSON(apiCall.getRecordEventListToFrom + '/' + dateFrom + '/' + dateTo).then(result => {
        const graphDataInfo = commonFunction.findGraphData(result, graphOption);
        viewModel.set('axisLegend', 'X axis = Time' + '             ' + 'Y axis = ' + graphOption.name);
        viewModel.set('xAxisData', 'readDateTime');
        viewModel.set('yAxisData', graphOption.databaseField);
        viewModel.set('graphData', graphDataInfo.graphDataArray);
        viewModel.set('min', graphDataInfo.min);
        viewModel.set('max', graphDataInfo.max);
        viewModel.set('graphMin', -5000);
        viewModel.set('graphMax', 5000);
        viewModel.set('graphMin', (graphDataInfo.min - 4).toFixed(0));
        viewModel.set('graphMax', (graphDataInfo.max + 4).toFixed(0));
        viewModel.set('graphYAxisInterval', graphDataInfo.yAxisInterval);
        viewModel.set('average', graphDataInfo.average);
        viewModel.set('graphHeader', graphOption.name);
        viewModel.set('graphSelected', graphOption.nameAbbreviated);
    }, error => {
        console.log(error);
    });
}