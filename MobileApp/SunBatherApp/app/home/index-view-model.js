import { Observable } from '@nativescript/core'
var commonFunction = require("../shared/commonFunctions");
var apiCall = require("../shared/apiConfig");
var apiRequests = require("../shared/apiRequests");
import { graphOptions }  from "../models/graphModel";

function indexPageIntialize(viewModel) {
    // needed for underline in header
    viewModel.set('headerSelected', 0); 
  
    // graph options list, default value loaded in screen is solar irridiance, refer to graphModel.js under Models folder
    var defaultSetting = 3;
    var graphOptionList = graphOptions();
    graphOptionList[defaultSetting].isSelected = true;
    viewModel.set('graphOptions', graphOptionList);
  
    // turn live data to true when testing live data, else it uses sample data from API 
    var liveData = apiCall.isLiveData; 
    var dateInfo = commonFunction.isLiveData(liveData);
    
    // populates data on graph and info
    apiRequests.getRecordEventList(dateInfo.dateYesterday, dateInfo.dateNow, viewModel, graphOptionList[defaultSetting], true);
    apiRequests.getMainPageData(viewModel);
}

export function UserViewModel() {
    const viewModel = new Observable();

    // initialise page
    indexPageIntialize(viewModel);

    return viewModel
} 
