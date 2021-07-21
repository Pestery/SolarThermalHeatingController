import { Http, Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

export function ConnectViewModel() {
    const viewModel = new Observable();
    viewModel.set('headerSelected', 4); // needed for underline in header

  

    return viewModel
}
