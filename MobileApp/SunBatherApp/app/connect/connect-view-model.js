import { Http, Observable } from '@nativescript/core'
var apiCall = require("../shared/apiConfig");
var commonFunction = require("../shared/commonFunctions");

export function ConnectViewModel() {
    const viewModel = new Observable();
    viewModel.set('headerSelected', 4); // needed for underline in header
    //var items = {};
    var countries = [
        { name: "Australia" },
        { name: "Belgium" },
        { name: "Bulgaria" },
        { name: "Canada" },
        { name: "Switzerland" },
        { name: "China" },
        { name: "Czech Republic" },
        { name: "Germany" },
        { name: "Spain" },
        { name: "Ethiopia" },
        { name: "Croatia" },
        { name: "Hungary" },
        { name: "Italy" },
        { name: "Jamaica" },
        { name: "Romania" },
        { name: "Russia" },
        { name: "United States" },
      ];

      viewModel.set('countries', countries)
  
      viewModel.onItemTap = (args) => {
        console.log('Item with index: ' + args.index + ' tapped');
      }
  

    return viewModel
}
