import { Http, Observable, ObservableArray } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

export function ConnectViewModel() {
    const viewModel = new Observable();
    const items = new ObservableArray();
    viewModel.set('headerSelected', 3); // needed for underline in header
    //var items = {};
    
    for (var loop = 0; loop < 20; loop++) {
        items.push("Item " + loop.toString());
    }

    console.log(items)

    viewModel.set("items", items);
    viewModel.set("selectedIndex", 15);

    viewModel.openDropDown = () => {
        console.log('I was tapped'); //this works
    }
    return viewModel
}
