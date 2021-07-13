import { Http, Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

export function UserViewModel() {
  const viewModel = new Observable();
  viewModel.set('headerSelected', 0); // needed for underline in header

  // finds getJSON from API
  viewModel.getRequest = () => {
    Http.getJSON(apiCall.getSystemStatus).then(result => {
      viewModel.set('pumpStatus', commonFunction.convertOnOff(result.pumpStatus));
      viewModel.set('setTemp', commonFunction.addCelcius(result.setTemperature));         
    }, error => {
      console.log(error);
    });

    Http.getJSON(apiCall.getRecordEventLatest).then(result => {
      viewModel.set('poolTemp', commonFunction.addCelcius(result.temperatureValueInput));
      viewModel.set('roofTemp', commonFunction.addCelcius(result.temperatureValueRoof));
      viewModel.set('currentUV', commonFunction.addUV(result.solarIrradiance));
    }, error => {
      console.log(error);
    });

    Http.getJSON(apiCall.getRecordEventList).then(result => {
      viewModel.set('tempGraph', commonFunction.convertDateTime(result));
    }, error => {
      console.log(error);
    });

    console.log('Perhaops');
    viewModel.set('currentUV', "perhaps");

  };
  viewModel.set('currentUV', 'pehaps');
  console.log('loasded')
  return viewModel
} 
