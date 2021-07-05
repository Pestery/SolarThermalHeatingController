import { Http } from '@nativescript/core';
import { Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

export function ContollerViewModel() {
  const viewModel = new Observable();
 
  viewModel.set('systemID', apiCall.SystemID);

  Http.getJSON(apiCall.GetSystemStatus).then(result => {
    //console.log(result);
    viewModel.set('pumpStatus', commonFunction.convertOnOff(result.pumpStatus));
    viewModel.set('setTemp', commonFunction.addCelcius(result.setTemperature));       
    viewModel.set('pumpMode', commonFunction.convertAutoManual(result.pumpMode))  
    viewModel.set('showDetails', result.pumpStatus)
  }, error => {
    console.log(error);
  });

  Http.getJSON(apiCall.GetRecordEventLatest).then(result => {
    //console.log(result);
    viewModel.set('livePoolTemp', commonFunction.addCelcius(result.temperatureValueInput));
  }, error => {
    console.log(error);
  });

  return viewModel
}
