var config = require("../../shared/config");
import { Http } from '@nativescript/core';
import { Observable } from '@nativescript/core'

// returns message
function getMessage(message) {
  return JSON.stringify(message);
}

export function UserViewModel() {
  const viewModel = new Observable()
   
  // finds getJSON from API
  viewModel.getRequest = () => {
    Http.getJSON(config.apiUrlGet).then(result => {
      console.log(JSON.stringify(result));
      viewModel.set('message', getMessage(result));
    }, error => {
      console.log(error);
    });
  };

  return viewModel
}
