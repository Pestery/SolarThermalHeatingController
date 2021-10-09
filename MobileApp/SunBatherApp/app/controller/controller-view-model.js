import { Http, Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");
var apiRequests = require("../shared/apiRequests");

function controllerPageInitialise(viewModel) {
  viewModel.set('headerSelected', 3); // needed for underline in header
  viewModel.set('systemID', apiCall.systemID);

  apiRequests.getSystemStatus(viewModel);
  apiRequests.getRecordEventLatest(viewModel);
}

function getViewModelData(viewModel) {
  var arrayData = [];
  arrayData[0] = commonFunction.convertToBool(viewModel.get('pumpMode'));
  arrayData[1] = commonFunction.convertToBool(viewModel.get('manualPumpStatus'));
  arrayData[2] = commonFunction.convertToBool(viewModel.get('pumpStatus'));
  arrayData[3] = viewModel.get('setTemp');

  return arrayData;
}

// main 
export function ContollerViewModel() {
  const viewModel = new Observable();
  controllerPageInitialise(viewModel);

  // updates set temperature
  viewModel.updateSetTemperature = () => {
    // get view data
    var viewModelDataArray = getViewModelData(viewModel);

    // update database info
    apiRequests.putSystemStatus(viewModelDataArray[0], viewModelDataArray[1], viewModelDataArray[2], viewModelDataArray[3]); 
  }

  // updates pump mode
  viewModel.updatePumpMode = () => {
    // get view data
    var viewModelDataArray = getViewModelData(viewModel);

    // update database info
    apiRequests.putSystemStatus(!viewModelDataArray[0], viewModelDataArray[1], viewModelDataArray[2], viewModelDataArray[3]); 

    // updates viewmodel for pumpMode
    viewModel.set('pumpMode', commonFunction.convertAutoManual(!viewModelDataArray[0]));
    viewModel.set('showDetails', viewModelDataArray[0]);
  };

  // updates pump status, function only works IF pump mode is on manual
  viewModel.updatePumpStatus = () => {
    if (!commonFunction.convertToBool(viewModel.get('pumpMode'))) {
      // get view data
      var viewModelDataArray = getViewModelData(viewModel);

      // update database info
      apiRequests.putSystemStatus(viewModelDataArray[0], !viewModelDataArray[1], viewModelDataArray[2], viewModelDataArray[3]); 
  
      // updates viewmodel for pumpMode
      viewModel.set('manualPumpStatus', commonFunction.convertOnOff(!viewModelDataArray[1]));
    }
  };

  return viewModel
}
