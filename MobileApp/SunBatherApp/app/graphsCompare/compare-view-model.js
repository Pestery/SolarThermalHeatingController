import { Observable } from '@nativescript/core'
var commonFunction = require("../shared/commonFunctions");
var apiRequests = require("../shared/apiRequests");
var apiCall = require("../shared/apiConfig");
import { compareOptions }  from "../models/compareModel";
import { competitorOptions }  from "../models/competitorModel";

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
    var graphOptionList = compareOptions();
    graphOptionList[viewModel.get('graphOptionSelected')].isSelected = true;
    viewModel.set('graphOptions', graphOptionList);

    // compare options list, default value loaded in screen is 
    viewModel.set('compareOptionSelected', 1)
    var compareOptionList = competitorOptions();
    compareOptionList[viewModel.get('compareOptionSelected')].isSelected = true;
    viewModel.set('compareOptions', compareOptionList);

    // turn live data to true when testing live data, else it uses sample data from API, this can be removed when product done
    var liveData = apiCall.isLiveData;
    var dateInfo = commonFunction.isLiveData(liveData);
    
    // for getRequest
    viewModel.set('storedDateFrom', dateInfo.dateYesterday); 
    viewModel.set('storedDateTo', dateInfo.dateNow);

    // display dates (MONTH must have + 1 for since months are done 0-11, not 1-12), also stops picker from selecting future dates
    viewModel.set('displayDateFrom', dateInfo.dateYesterdayConvert.getDate() + '/' + (dateInfo.dateYesterdayConvert.getMonth() + 1) + '/' + dateInfo.dateYesterdayConvert.getFullYear());
    viewModel.set('displayDateTo', dateInfo.dateNowConvert.getDate() + '/' + (dateInfo.dateNowConvert.getMonth() + 1) + '/' + dateInfo.dateNowConvert.getFullYear());
    viewModel.set('maxDate', dateInfo.dateNow);

    // populates data on graph
    apiRequests.getRecordEventList(dateInfo.dateYesterday, dateInfo.dateNow, viewModel, graphOptionList[viewModel.get('graphOptionSelected')], false, compareOptionList[viewModel.get('compareOptionSelected')]);
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
        var graphOptionList = compareOptions();
        var compareOptionList = competitorOptions();
        viewModel.set('showDatePicker', false);
        viewModel.set('showData', true);
        viewModel.set('showGraphPicker', false);
        viewModel.set('showCompetitorPicker', false);
        viewModel.set('graphSelected', graphOptionList[viewModel.get('graphOptionSelected')].nameAbbreviated);
        viewModel.set('competitorSelected', compareOptionList[viewModel.get('compareOptionSelected')].nameAbbreviated);
    }
    
    viewModel.updateGraph = () => {
        var graphOptionList = compareOptions();
        var compareOptionList = competitorOptions();
        var getDateFrom = viewModel.get('storedDateFrom');
        var getDateTo = viewModel.get('storedDateTo');
        apiRequests.getRecordEventList(getDateFrom, getDateTo, viewModel, graphOptionList[viewModel.get('graphOptionSelected')], false, compareOptionList[viewModel.get('compareOptionSelected')]);
    }
    
      // creates a new object everytime, but its the only way, obervable array is not good - jack 
    viewModel.onItemTapGraph = (args) => {
        var graphOptionList = compareOptions();
        graphOptionList[args.index].isSelected = true;
        viewModel.set('graphOptions', graphOptionList);
        viewModel.set('graphOptionSelected', args.index);
    }

    viewModel.onItemTapCompare = (args) => {
        var compareOptionList = competitorOptions();
        compareOptionList[args.index].isSelected = true;
        viewModel.set('compareOptions', compareOptionList);
        viewModel.set('compareOptionSelected', args.index);
    }

    return viewModel; 
}
