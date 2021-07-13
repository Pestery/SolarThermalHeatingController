import { Http } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

export function getRecordEventList(dateFrom, dateTo, viewModel) {
    // console.log(Object.keys(result[0])) can be used to find array property names, could be useful in future for decoupling option data from database - JACK
    Http.getJSON(apiCall.getRecordEventListToFrom + '/' + dateFrom + '/' + dateTo).then(result => {
        const graphDataInfo = commonFunction.findGraphData(result);
        viewModel.set('graphData', graphDataInfo.graphDataArray);
        viewModel.set('axisLegend', 'X axis = Time               Y axis = Temperature of Roof');
        viewModel.set('xAxisData', 'readDateTime');
        viewModel.set('yAxisData', 'temperatureValueRoof');
        viewModel.set('min', graphDataInfo.min);
        viewModel.set('max', graphDataInfo.max);
        viewModel.set('average', graphDataInfo.average);
    }, error => {
        console.log(error);
    });
}