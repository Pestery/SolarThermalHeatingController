import { Http } from '@nativescript/core';
import { Observable } from '@nativescript/core'
var config = require("../shared/config");
var commonFunction = require("../shared/commonFunctions");

export function UserViewModel() {
  const viewModel = new Observable()

  // finds getJSON from API
  viewModel.getRequest = () => {
    Http.getJSON(config.apiGetSystemStatus).then(result => {
      //console.log(result);
      viewModel.set('pumpStatus', commonFunction.convertBool(result.pumpStatus));
      viewModel.set('setTemp', commonFunction.addCelcius(result.setTemperature));         
    }, error => {
      console.log(error);
    });

    Http.getJSON(config.apiGetRecordEventLatest).then(result => {
     // console.log(result);
      viewModel.set('poolTemp', commonFunction.addCelcius(result.temperatureValueInput));
      viewModel.set('roofTemp', commonFunction.addCelcius(result.temperatureValueRoof));
      viewModel.set('currentUV', commonFunction.addUV(result.solarIrradiance));
    }, error => {
      console.log(error);
    });

    Http.getJSON(config.apiGetRecordEvent).then(result => {
      console.log(result);
      viewModel.set('categoricalSource', commonFunction.convertDateTime(result));
    }, error => {
      console.log(error);
    });

    viewModel.set('currentUV', "perhaps");
  };

  return viewModel
}
