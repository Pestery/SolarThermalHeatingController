import { Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");
var apiRequests = require("../shared/apiRequests");

function pickerFunction(data, viewModel) {
    const datePicker = data.object
    datePicker.on('dateChange', args => {
        var updatedTimeISO = new Date(args.value).toISOString();
        var updatedTime = new Date(args.value);
        if (viewModel.get('dateFromClicked')) {
            viewModel.set('storedDateFrom', updatedTimeISO);
            viewModel.set('displayDateFrom', updatedTime.getDate() + '/' + (updatedTime.getMonth() + 1) + '/' + updatedTime.getFullYear());
    
        } else {
            viewModel.set('storedDateTo', updatedTimeISO);
            viewModel.set('displayDateTo', updatedTime.getDate() + '/' + (updatedTime.getMonth() + 1) + '/' + updatedTime.getFullYear());
        }
    })
}

function comparePageIntialize(viewModel) { 
    viewModel.set('headerSelected', 2); 
    viewModel.set('showDatePicker', false);
    viewModel.set('showData', true);
    viewModel.set('showGraphPicker', false);

    // graph options list, default value loaded in screen is emissions
    viewModel.set('graphOptionSelected', 1)
    var graphOptionList = commonFunction.graphOptionsCompare();
    graphOptionList[viewModel.get('graphOptionSelected')].isSelected = true;
    viewModel.set('graphOptions', graphOptionList);

    // compare options list, default value loaded in screen is 
    viewModel.set('compareOptionSelected', 1)
    var compareOptionList = commonFunction.competitorOptions();
    compareOptionList[viewModel.get('compareOptionSelected')].isSelected = true;
    viewModel.set('compareOptions', compareOptionList);

    // turn live data to true when testing live data, else it uses sample data from API 
    var liveData = false;
    var dateNow;
    var dateYesterday;
    var dateNowConvert;
    var dateYesterdayConvert;

    // iso string is good and prevents time zone hassle when converting to C#
    if (liveData) {
        dateNow = new Date().toISOString();
        dateYesterday = new Date(Date.now() - 1 * 24 * 60 * 60 * 1000).toISOString();
        dateNowConvert = new Date(dateNow);
        dateYesterdayConvert = new Date(dateYesterday); 
    } else {
        dateNow = new Date("07/11/2021 12:00").toISOString();
        dateYesterday = new Date("07/10/2021 12:00").toISOString();
        dateNowConvert = new Date(dateNow);
        dateYesterdayConvert = new Date(dateYesterday); 
    }
    
    // for getRequest
    viewModel.set('storedDateFrom', dateYesterday); 
    viewModel.set('storedDateTo', dateNow);

    // display dates (MONTH must have + 1 for since months are done 0-11, not 1-12)
    viewModel.set('displayDateFrom', dateYesterdayConvert.getDate() + '/' + (dateYesterdayConvert.getMonth() + 1) + '/' + dateYesterdayConvert.getFullYear());
    viewModel.set('displayDateTo', dateNowConvert.getDate() + '/' + (dateNowConvert.getMonth() + 1) + '/' + dateNowConvert.getFullYear());

    // populates data on graph
    apiRequests.getRecordEventList(dateYesterday, dateNow, viewModel, graphOptionList[viewModel.get('graphOptionSelected')], false);
    apiRequests.getCompetitorEventList(dateYesterday, dateNow, viewModel, graphOptionList[viewModel.get('graphOptionSelected')], compareOptionList[viewModel.get('compareOptionSelected')]);
}

export function CompareViewModel() {
    const viewModel = new Observable();  
    comparePageIntialize(viewModel);

    viewModel.onDatePickerLoaded = (data) => { 
        pickerFunction(data, viewModel);
    }

    viewModel.dateFromClicked = () => {
        viewModel.set('showDatePicker', true);
        viewModel.set('showData', false);
        viewModel.set('dateFromClicked', true);
    }
    
    viewModel.dateToClicked = () => { 
        viewModel.set('showDatePicker', true);
        viewModel.set('showData', false);
        viewModel.set('dateFromClicked', false);
    }
    
    viewModel.graphPickerClicked = () => {
        viewModel.set('showData', false);
        viewModel.set('showGraphPicker', true);
    }

    viewModel.graphCompetitorClicked = () => {
      viewModel.set('showData', false);
      viewModel.set('showCompetitorPicker', true);
  }
    
    viewModel.hidePicker = () => {
        var graphOptionList = commonFunction.graphOptionsCompare();
        var compareOptionList = commonFunction.competitorOptions();
        viewModel.set('showDatePicker', false);
        viewModel.set('showData', true);
        viewModel.set('showGraphPicker', false);
        viewModel.set('showCompetitorPicker', false);
        viewModel.set('graphSelected', graphOptionList[viewModel.get('graphOptionSelected')].nameAbbreviated);
        viewModel.set('competitorSelected', compareOptionList[viewModel.get('compareOptionSelected')].nameAbbreviated);
    }
    
    viewModel.updateGraph = () => {
        var graphOptionList = commonFunction.graphOptionsCompare();
        var compareOptionList = commonFunction.competitorOptions();
        var getDateFrom = viewModel.get('storedDateFrom');
        var getDateTo = viewModel.get('storedDateTo');
        apiRequests.getRecordEventList(getDateFrom, getDateTo, viewModel, graphOptionList[viewModel.get('graphOptionSelected')], false);
        apiRequests.getCompetitorEventList(getDateFrom, getDateTo, viewModel, graphOptionList[viewModel.get('graphOptionSelected')], compareOptionList[viewModel.get('compareOptionSelected')]);
    }
    
      // creates a new object everytime, but its the only way, obervable array is not good - jack 
    viewModel.onItemTapGraph = (args) => {
        var graphOptionList = commonFunction.graphOptionsCompare();
        graphOptionList[args.index].isSelected = true;
        viewModel.set('graphOptions', graphOptionList);
        viewModel.set('graphOptionSelected', args.index);
    }

    viewModel.onItemTapCompare = (args) => {
        var compareOptionList = commonFunction.competitorOptions();
        compareOptionList[args.index].isSelected = true;
        viewModel.set('compareOptions', compareOptionList);
        viewModel.set('compareOptionSelected', args.index);
    }

    return viewModel; 
}
