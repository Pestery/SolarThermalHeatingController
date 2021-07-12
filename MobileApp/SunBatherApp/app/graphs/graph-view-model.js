import { Http, Observable } from '@nativescript/core'
import { convertHSLToRGBColor } from '@nativescript/core/css/parser';
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");
var apiRequests = require("../shared/apiRequests");

function pickerFunction(data, viewModel) {
  const datePicker = data.object
  datePicker.on('dateChange', args => {
    if (viewModel.get('dateFromClicked')) {
      viewModel.set('storedDateFrom', new Date(args.value).toISOString());
    } else {
      viewModel.set('storedDateTo', new Date(args.value).toISOString());
    }
  })
}

function graphPageIntialize(viewModel) {
  // needed for underline in header
  viewModel.set('headerSelected', 1); 
  viewModel.set('showPicker', false);

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
  apiRequests.getRecordEventList(dateYesterday, dateNow, viewModel);
}

export function GraphViewModel() {
  const viewModel = new Observable(); 
  graphPageIntialize(viewModel);

  viewModel.onDatePickerLoaded = (data) => { 
    pickerFunction(data, viewModel);
  }

  viewModel.dateFromClicked = () => {
    viewModel.set('showPicker', true);
    viewModel.set('dateFromClicked', true);
  }

  viewModel.dateToClicked = () => { 
    viewModel.set('showPicker', true);
    viewModel.set('dateFromClicked', false);
  }

  viewModel.hidePicker = () => {
    viewModel.set('showPicker', false);
  }

  viewModel.updateGraph = () => {
    Http.getJSON(apiCall.getRecordEventListToFrom + '/' + viewModel.get('storedDateFrom') + '/' + viewModel.get('storedDateTo')).then(result => {
      //console.log(result);
    }, error => {
      console.log(error);
    });
  }
  return viewModel
}