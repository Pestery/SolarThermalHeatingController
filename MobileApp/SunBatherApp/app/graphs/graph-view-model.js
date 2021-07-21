import { Observable } from '@nativescript/core'
var apiRequests = require("../shared/apiRequests");
var commonFunction = require("../shared/commonFunctions");
import { graphOptions }  from "../models/graphModel";

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

function graphPageIntialize(viewModel) {
  // needed for underline in header
  viewModel.set('headerSelected', 1); 
  viewModel.set('showDatePicker', false);
  viewModel.set('showData', true);
  viewModel.set('showGraphPicker', false);

  // graph options list, default value loaded in screen is roof temp
  viewModel.set('graphOptionSelected', 1)
  var graphOptionList = graphOptions();
  graphOptionList[viewModel.get('graphOptionSelected')].isSelected = true;
  viewModel.set('graphOptions', graphOptionList);

  // turn live data to true when testing live data, else it uses sample data from API, this can be removed when product done
  var liveData = false;
  var dateInfo = commonFunction.isLiveData(liveData);

  // for getRequest
  viewModel.set('storedDateFrom', dateInfo.dateYesterday); 
  viewModel.set('storedDateTo', dateInfo.dateNow);

  // display dates (MONTH must have + 1 for since months are done 0-11, not 1-12)
  viewModel.set('displayDateFrom', dateInfo.dateYesterdayConvert.getDate() + '/' + (dateInfo.dateYesterdayConvert.getMonth() + 1) + '/' + dateInfo.dateYesterdayConvert.getFullYear());
  viewModel.set('displayDateTo', dateInfo.dateNowConvert.getDate() + '/' + (dateInfo.dateNowConvert.getMonth() + 1) + '/' + dateInfo.dateNowConvert.getFullYear());

  // populates data on graph
  apiRequests.getRecordEventList(dateInfo.dateYesterday, dateInfo.dateNow, viewModel, graphOptionList[viewModel.get('graphOptionSelected')], true);
}

export function GraphViewModel() {
  const viewModel = new Observable(); 
  graphPageIntialize(viewModel);

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

  viewModel.hidePicker = () => {
    viewModel.set('showDatePicker', false);
    viewModel.set('showData', true);
    viewModel.set('showGraphPicker', false);
  }

  viewModel.updateGraph = () => {
    var graphOptionList = graphOptions();
    var getDateFrom = viewModel.get('storedDateFrom');
    var getDateTo = viewModel.get('storedDateTo');
    apiRequests.getRecordEventList(getDateFrom, getDateTo, viewModel, graphOptionList[viewModel.get('graphOptionSelected')], true);
  }

  // creates a new object everytime, but its the only way, obervable array is not good - jack 
  viewModel.onItemTap = (args) => {
    var graphOptionList = graphOptions();
    graphOptionList[args.index].isSelected = true;
    viewModel.set('graphOptions', graphOptionList);
    viewModel.set('graphOptionSelected', args.index);
    viewModel.set('graphSelected', graphOptionList[viewModel.get('graphOptionSelected')].nameAbbreviated);
  }

  return viewModel
}