import { Http } from '@nativescript/core';
import { Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

export function GraphViewModel() {
  const viewModel = new Observable();
  viewModel.set('headerSelected', 1); // needed for underline in header

  viewModel.onLoad = () => { 
    console.log('ppp')
  }

  viewModel.dateChanged = () => { 
    console.log('sss')
  }

  return viewModel
}