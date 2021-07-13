var config = require("../../shared/config");
import { Http } from '@nativescript/core';
import { Observable } from '@nativescript/core'


// returns message
function getMessage(message) {
  var sentence = "ID: " + message.id + "\n Value: " + message.value + "\n DateTime: " + message.readDate;
  return sentence;
}

export function UserViewModel() {
  const viewModel = new Observable()
   
  // finds getJSON from API
  viewModel.getRequest = () => {
    console.log("hhhhh")
    Http.getJSON(config.apiMobileUrlGetLatest).then(result => {
      console.log(result);
      viewModel.set('message', getMessage(result));
    }, error => {
      console.log(error);
    });
  };

  return viewModel
}
