import { Http, Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

// peforms put requests for the database
function putRequest(updatedPumpMode, updatePumpStatus, updateSetTemperature) {
  Http.request({
    url: apiCall.putSystemStatus,
    method: "PUT",
    headers: { "Content-Type": "application/json" },
    content: JSON.stringify({
      Id: apiCall.KeyID,
      SystemIdentityID: apiCall.KeyID,
      PumpMode: updatedPumpMode,
      SetTemperature: updateSetTemperature,
      PumpStatus: updatePumpStatus,
    }),
  }).then(response => {
    var result = response.content.toJSON();
    console.log(result)
  }, error => { 
      console.error(error);
  });
}

// main 
export function ContollerViewModel() {
  const viewModel = new Observable();
  viewModel.set('headerSelected', 2); // needed for underline in header
  viewModel.set('systemID', apiCall.SystemID);

  Http.getJSON(apiCall.getSystemStatus).then(result => {
    //console.log(result);
    viewModel.set('pumpStatus', commonFunction.convertOnOff(result.pumpStatus));
    viewModel.set('setTemp', result.setTemperature);       
    viewModel.set('pumpMode', commonFunction.convertAutoManual(result.pumpMode));
    viewModel.set('showDetails', !result.pumpMode);
  }, error => {
    console.log(error);
  });

  Http.getJSON(apiCall.getRecordEventLatest).then(result => {
    //console.log(result);
    viewModel.set('livePoolTemp', commonFunction.addCelcius(result.temperatureValueInput));
  }, error => {
    console.log(error);
  });

  // updates set temperature
  viewModel.updateSetTemperature = () => {
    var updatedPumpMode = commonFunction.convertToBool(viewModel.get('pumpMode'));
    var pumpStatus = commonFunction.convertToBool(viewModel.get('pumpStatus'));
    var updateSetTemperature = viewModel.get('setTemp');

    // update database info
    putRequest(updatedPumpMode, pumpStatus, updateSetTemperature);

    // updates viewmodel for set temp
    viewModel.set('setTemp', updateSetTemperature);
  }

  // updates pump mode
  viewModel.updatePumpMode = () => {
    var updatedPumpMode = !commonFunction.convertToBool(viewModel.get('pumpMode'));
    var pumpStatus = commonFunction.convertToBool(viewModel.get('pumpStatus'));
    var updateSetTemperature = viewModel.get('setTemp');

    // update database info
    putRequest(updatedPumpMode, pumpStatus, updateSetTemperature);

    // updates viewmodel for pumpMode
    viewModel.set('pumpMode', commonFunction.convertAutoManual(updatedPumpMode));
    viewModel.set('showDetails', !updatedPumpMode);
  };

  // updates pump status, function only works IF pump mode is on manual
  viewModel.updatePumpStatus = () => {
    console.log('hello')
    if (!commonFunction.convertToBool(viewModel.get('pumpMode'))) {
      var pumpMode = commonFunction.convertToBool(viewModel.get('pumpMode'));
      var updatedPumpStatus = !commonFunction.convertToBool(viewModel.get('pumpStatus'));
      var updateSetTemperature = viewModel.get('setTemp');

      // update database info
      putRequest(pumpMode, updatedPumpStatus, updateSetTemperature);
  
      // updates viewmodel for pumpMode
      viewModel.set('pumpStatus', commonFunction.convertOnOff(updatedPumpStatus));
      viewModel.set('showDetails', !pumpMode);
    }
  };

  return viewModel
}
