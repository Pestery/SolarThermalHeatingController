import { DatePicker, Http, Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

function pickerFunction(data, viewModel) {
  const datePicker = data.object
  datePicker.on('dateChange', args => {
    console.log(args.value);
    console.log(Date.parse(args.value));
    if (viewModel.get('dateFromClicked')) {
      console.log('dateFrom');
      viewModel.set('storedDateFrom', Date.parse(args.value));
    } else {
      console.log('dateTo');
      viewModel.set('storedDateTo', Date.parse(args.value));
    }
  })
}

export function GraphViewModel() {
  const viewModel = new Observable(); 
  viewModel.set('headerSelected', 1); // needed for underline in header
  //viewModel.set('minDate', )

  viewModel.set('showPicker', false);
  // useful for the future
  // viewModel.set('maxDate', new Date(new Date().getFullYear(), new Date().getMonth(), new Date().getDate())) 

  console.log( new Date(new Date().getFullYear(), new Date().getMonth(), new Date().getDate()))
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

  // finds last 24hr data
  Http.getJSON(apiCall.getRecordEventListToFrom + '/' + (new Date().getTime() - 86400000) + '/' + new Date().getTime()).then(result => {
    //console.log(result);
  }, error => {
    console.log(error);
  });

  viewModel.updateGraph = () => {
    Http.getJSON(apiCall.getRecordEventListToFrom + '/' + viewModel.get('storedDateFrom') + '/' + viewModel.get('storedDateTo')).then(result => {
      //console.log(result);
    }, error => {
      console.log(error);
    });
  }
  return viewModel
}