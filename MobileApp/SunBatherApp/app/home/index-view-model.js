import { Http, Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");
var apiRequests = require("../shared/apiRequests");
import { graphOptions }  from "../models/graphModel";

function indexPageIntialize(viewModel) {
    // needed for underline in header
    viewModel.set('headerSelected', 0); 
  
    // graph options list, default value loaded in screen is roof temp
    var defualtSetting = 1;
    var graphOptionList = graphOptions();
    graphOptionList[defualtSetting].isSelected = true;
    viewModel.set('graphOptions', graphOptionList);
  
    // turn live data to true when testing live data, else it uses sample data from API 
    var liveData = false;
    var dateNow;
    var dateYesterday;
  
     // iso string is good and prevents time zone hassle when converting to C#
    if (liveData) {
      dateNow = new Date().toISOString();
      dateYesterday = new Date(Date.now() - 1 * 24 * 60 * 60 * 1000).toISOString();
    } else {
      dateNow = new Date("07/11/2021 12:00").toISOString();
      dateYesterday = new Date("07/10/2021 12:00").toISOString();
    }
    
    console.log('entered function')
    // populates data on graph
    apiRequests.getRecordEventList(dateYesterday, dateNow, viewModel, graphOptionList[defualtSetting], true);
}

export function UserViewModel() {
    const viewModel = new Observable();
    indexPageIntialize(viewModel);

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

    return viewModel
} 
