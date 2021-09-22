const CF = require('../SunBatherApp/app/shared/commonFunctionsMod');
const graphModel = require("../SunBatherApp/app/models/graphModelMod");

test('Test Convert to Bool', () => {
  expect(CF.convertToBool("autO")).toBe(true);
  expect(CF.convertToBool("On")).toBe(true);
  expect(CF.convertToBool("Manual")).toBe(false);
  expect(CF.convertToBool(undefined)).toBe(false);
  expect(CF.convertToBool("autO")).not.toBe(false);
  expect(CF.convertToBool("On")).not.toBe(false);
  expect(CF.convertToBool("Manual")).not.toBe(true);
  expect(CF.convertToBool(undefined)).not.toBe(true);
});

test('Test Convert bool to On/Off', () => {
  expect(CF.convertOnOff(true)).toBe("On");
  expect(CF.convertOnOff(false)).toBe("Off");
  expect(CF.convertOnOff(true)).not.toBe("Off");
  expect(CF.convertOnOff(false)).not.toBe("On");
});

test('Test Convert bool to auto/manual', () => {
  expect(CF.convertAutoManual(true)).toBe("Auto");
  expect(CF.convertAutoManual(false)).toBe("Manual");
  expect(CF.convertAutoManual(true)).not.toBe("Manual");
  expect(CF.convertAutoManual(false)).not.toBe("Auto");
});

test('Test Add Celcius Symbol', () => {
  expect(CF.addCelcius("25")).toBe("25 C");
  expect(CF.addCelcius("25")).not.toBe("25C");
  expect(CF.addCelcius("25")).not.toBe("24 C");
});

test('Test Add W/M^2 Symbol', () => {
  expect(CF.addUV("25")).toBe("25 W/M^2");
  expect(CF.addUV("25")).not.toBe("25W/M^2");
  expect(CF.addUV("25")).not.toBe("24 W/M^2");
});

test('Test Average Calculator', () => {
  expect(CF.averageRound(10, 2)).toBe("5.0"); // check if average correctly
  expect(CF.averageRound(10, 3)).toBe("3.3"); // check if rounds correctly (would be 3.3333 if no rounding)
  expect(CF.averageRound(10, 0)).toBe(0); // if divide by zero or less, should always return 0
  expect(CF.averageRound(10, -10)).toBe(0);
  expect(CF.averageRound(-10, 10)).toBe("-1.0"); // check if average negative numbers correctly
  expect(CF.averageRound(10, 2)).not.toBe("4.0");
  expect(CF.averageRound(10, 3)).not.toBe("3.33");
  expect(CF.averageRound(10, 0)).not.toBe(10);
  expect(CF.averageRound(10, -10)).not.toBe(-1);
  expect(CF.averageRound(-10, 10)).not.toBe("1.0");
});

// Returns smallest number
test('Test Minimum function', () => {
  expect(CF.findMin(11, 11)).toBe(11); 
  expect(CF.findMin(9, 11)).toBe(9); 
  expect(CF.findMin(12, 8)).toBe(8); 
  expect(CF.findMin(-2, -2)).toBe(-2); 
  expect(CF.findMin(-3, -2)).toBe(-3); 
  expect(CF.findMin(-1, -2)).toBe(-2); 
  expect(CF.findMin(11, 11)).not.toBe(10); 
  expect(CF.findMin(9, 11)).not.toBe(10); 
  expect(CF.findMin(12, 8)).not.toBe(10); 
  expect(CF.findMin(-2, -2)).not.toBe(-10); 
  expect(CF.findMin(-3, -2)).not.toBe(-10); 
  expect(CF.findMin(-1, -2)).not.toBe(-10); 
});

// Returns Biggest number
test('Test Maxiumim function', () => {
  expect(CF.findMax(11, 11)).toBe(11); 
  expect(CF.findMax(9, 11)).toBe(11); 
  expect(CF.findMax(12, 8)).toBe(12); 
  expect(CF.findMax(-2, -2)).toBe(-2); 
  expect(CF.findMax(-3, -2)).toBe(-2); 
  expect(CF.findMax(-1, -2)).toBe(-1); 
  expect(CF.findMax(11, 11)).not.toBe(10); 
  expect(CF.findMax(9, 11)).not.toBe(10); 
  expect(CF.findMax(12, 8)).not.toBe(10); 
  expect(CF.findMax(-2, -2)).not.toBe(-10); 
  expect(CF.findMax(-3, -2)).not.toBe(-10); 
  expect(CF.findMax(-1, -2)).not.toBe(-10); 
});

// loops through data array
(test('Test Maxiumim function', () => {
  // Initialise Array
  var testArray = [
      { ID: 1, name: "temp1", readDateTime: new Date("07/11/2021 12:00"), temperatureIn: 10, temperatureOut: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date("07/11/2021 13:00"), temperatureIn: 15, temperatureOut: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date("07/11/2021 14:00"), temperatureIn: 20, temperatureOut: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date("07/11/2021 15:00"), temperatureIn: 25, temperatureOut: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date("07/11/2021 16:00"), temperatureIn: 30, temperatureOut: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date("07/11/2021 17:00"), temperatureIn: 35, temperatureOut: 30 }
    ];
  // checks temperature in
  expect(CF.loopThroughData(testArray, "temperatureIn")).toEqual({ 
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureIn: 10, temperatureOut: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureIn: 15, temperatureOut: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureIn: 20, temperatureOut: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureIn: 25, temperatureOut: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureIn: 30, temperatureOut: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureIn: 35, temperatureOut: 30 }
    ],
    min: 10,
    max: 35,
    average: "22.5"
  })
  // checks temperature out
  expect(CF.loopThroughData(testArray, "temperatureOut")).toEqual({ 
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureIn: 10, temperatureOut: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureIn: 15, temperatureOut: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureIn: 20, temperatureOut: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureIn: 25, temperatureOut: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureIn: 30, temperatureOut: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureIn: 35, temperatureOut: 30 }
    ],
    min: 5,
    max: 30,
    average: "17.5"
  })
  // checks if graphDataArray wrong
  expect(CF.loopThroughData(testArray, "temperatureOut")).not.toEqual({ 
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureIn: 10, temperatureOut: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureIn: 22, temperatureOut: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureIn: 20, temperatureOut: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureIn: 25, temperatureOut: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureIn: 30, temperatureOut: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureIn: 35, temperatureOut: 30 }
    ],
    min: 5,
    max: 30,
    average: "17.5"
  })     
  // checks if min wrong
  expect(CF.loopThroughData(testArray, "temperatureOut")).not.toEqual({ 
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureIn: 10, temperatureOut: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureIn: 15, temperatureOut: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureIn: 20, temperatureOut: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureIn: 25, temperatureOut: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureIn: 30, temperatureOut: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureIn: 35, temperatureOut: 30 }
    ],
    min: 10,
    max: 30,
    average: "17.5"
  })        
  // checks if max wrong
  expect(CF.loopThroughData(testArray, "temperatureOut")).not.toEqual({ 
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureIn: 10, temperatureOut: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureIn: 22, temperatureOut: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureIn: 20, temperatureOut: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureIn: 25, temperatureOut: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureIn: 30, temperatureOut: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureIn: 35, temperatureOut: 30 }
    ],
    min: 5,
    max: 35,
    average: "17.5"
  })        
  // checks if average wrong
  expect(CF.loopThroughData(testArray, "temperatureOut")).not.toEqual({ 
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureIn: 10, temperatureOut: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureIn: 22, temperatureOut: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureIn: 20, temperatureOut: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureIn: 25, temperatureOut: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureIn: 30, temperatureOut: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureIn: 35, temperatureOut: 30 }
    ],
    min: 5,
    max: 30,
    average: "22.5"
  })                                                       
}));

// Returns time period, JEST IS AMERICAN DATE
test('Test time period function', () => {
  var graphDataInfo = {};

  // check for day interval
  expect(CF.timePeriod(new Date(new Date("07/10/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).toEqual({
    xAxisInterval: 6,
    xAxisUnit: "Hour",
    xAxisFormat: "HH:mm"
  }); 
  // check for 3 day interval
  expect(CF.timePeriod(new Date(new Date("07/09/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).toEqual({
    xAxisInterval: 12,
    xAxisUnit: "Hour",
    xAxisFormat: "dd/MM HH:mm"
  }); 
  // check for 4-10 day interval
  expect(CF.timePeriod(new Date(new Date("07/02/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).toEqual({
    xAxisInterval: 2,
    xAxisUnit: "Day",
    xAxisFormat: "dd/MM"
  }); 
  // check for 10-30 day interval
  expect(CF.timePeriod(new Date(new Date("06/28/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).toEqual({
    xAxisInterval: 5,
    xAxisUnit: "Day",
    xAxisFormat: "dd/MM"
  }); 
  // check for 1 month interval
  expect(CF.timePeriod(new Date(new Date("06/06/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).toEqual({
    xAxisInterval: 6,
    xAxisUnit: "Day",
    xAxisFormat: "dd/MM"
  }); 
  // check for greater than a month
  expect(CF.timePeriod(new Date(new Date("04/10/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).toEqual({
    xAxisInterval: 15,
    xAxisUnit: "Day",
    xAxisFormat: "dd/MM"
  }); 
  // check for day interval to be false
  expect(CF.timePeriod(new Date(new Date("07/09/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).not.toEqual({
    xAxisInterval: 6,
    xAxisUnit: "Hour",
    xAxisFormat: "HH:mm"
  }); 
  // check for 3 day interval to be false
  expect(CF.timePeriod(new Date(new Date("07/05/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).not.toEqual({
    xAxisInterval: 12,
    xAxisUnit: "Hour",
    xAxisFormat: "dd/MM HH:mm"
  });
  // check for 4-10 day interval to be false
  expect(CF.timePeriod(new Date(new Date("07/10/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).not.toEqual({
    xAxisInterval: 2,
    xAxisUnit: "Day",
    xAxisFormat: "dd/MM"
  }); 
  // check for 10-30 day interval to be false
  expect(CF.timePeriod(new Date(new Date("06/07/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).not.toEqual({
    xAxisInterval: 5,
    xAxisUnit: "Day",
    xAxisFormat: "dd/MM"
  });  
  // check for 1 month interval to be false
  expect(CF.timePeriod(new Date(new Date("07/06/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).not.toEqual({
    xAxisInterval: 6,
    xAxisUnit: "Day",
    xAxisFormat: "dd/MM"
  }); 
  // check for greater than a month to be false
  expect(CF.timePeriod(new Date(new Date("07/10/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).not.toEqual({
    xAxisInterval: 15,
    xAxisUnit: "Day",
    xAxisFormat: "dd/MM"
  }); 
  // check to see if function false if dates are back to front
  expect(CF.timePeriod(new Date(new Date("07/14/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).toEqual({
    xAxisInterval: 6,
    xAxisUnit: "Hour",
    xAxisFormat: "HH:mm"
  }); 
  // check to see if function false if dates are back to front to be false
  expect(CF.timePeriod(new Date(new Date("07/14/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), graphDataInfo)).not.toEqual({
    xAxisInterval: 5,
    xAxisUnit: "Hour",
    xAxisFormat: "HH:mm"
  }); 
});

// used to find all data from an array and put it into usable graph data in nativescript chart
test('Test findGraphData function', () => {
  // Initialise Array
  var testArray = [
    { ID: 1, name: "temp1", readDateTime: new Date("07/11/2021 12:00"), temperatureValueInput: 10, temperatureValueOutput: 5 },
    { ID: 2, name: "temp2", readDateTime: new Date("07/11/2021 13:00"), temperatureValueInput: 15, temperatureValueOutput: 10 },
    { ID: 3, name: "temp3", readDateTime: new Date("07/11/2021 14:00"), temperatureValueInput: 20, temperatureValueOutput: 15 },
    { ID: 4, name: "temp4", readDateTime: new Date("07/11/2021 15:00"), temperatureValueInput: 25, temperatureValueOutput: 20 },
    { ID: 5, name: "temp5", readDateTime: new Date("07/11/2021 16:00"), temperatureValueInput: 30, temperatureValueOutput: 25 },
    { ID: 6, name: "temp6", readDateTime: new Date("07/11/2021 17:00"), temperatureValueInput: 35, temperatureValueOutput: 30 }
  ];

  var varGraphModel = graphModel.graphOptions();

  expect(CF.findGraphData(new Date(new Date("07/10/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), testArray, varGraphModel[0])).toEqual({
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureValueInput: 10, temperatureValueOutput: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureValueInput: 15, temperatureValueOutput: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureValueInput: 20, temperatureValueOutput: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureValueInput: 25, temperatureValueOutput: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureValueInput: 30, temperatureValueOutput: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureValueInput: 35, temperatureValueOutput: 30 }
    ],
    min: 10,
    max: 35,
    average: "22.5",
    yAxisInterval: "8", //((max - min) / 3) to zero decimcal places
    xAxisInterval: 6,
    xAxisUnit: "Hour",
    xAxisFormat: "HH:mm"
  }); 

  expect(CF.findGraphData(new Date(new Date("07/10/2021 12:00").toISOString()), new Date(new Date("07/11/2021 12:00").toISOString()), testArray, varGraphModel[1])).toEqual({
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureValueInput: 10, temperatureValueOutput: 5 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureValueInput: 15, temperatureValueOutput: 10 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureValueInput: 20, temperatureValueOutput: 15 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureValueInput: 25, temperatureValueOutput: 20 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureValueInput: 30, temperatureValueOutput: 25 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureValueInput: 35, temperatureValueOutput: 30 }
    ],
    min: 5,
    max: 30,
    average: "17.5",
    yAxisInterval: "8", //((max - min) / 3) to zero decimcal places
    xAxisInterval: 6,
    xAxisUnit: "Hour",
    xAxisFormat: "HH:mm"
  }); 
});

// used to set the app to a set date or live data, set date is used for testing
test('Test isLiveData function', () => {
  expect(CF.isLiveData(true)).toEqual({
    dateNow: new Date().toISOString(),
    dateYesterday: new Date(Date.now() - 1 * 24 * 60 * 60 * 1000).toISOString(),
    dateNowConvert: new Date(),
    dateYesterdayConvert: new Date(Date.now() - 1 * 24 * 60 * 60 * 1000)
  }); 
  expect(CF.isLiveData(false)).toEqual({
    dateNow: new Date("07/11/2021 12:00").toISOString(),
    dateYesterday: new Date("07/10/2021 12:00").toISOString(),
    dateNowConvert: new Date("07/11/2021 12:00"),
    dateYesterdayConvert: new Date("07/10/2021 12:00")
  }); 
  expect(CF.isLiveData(true)).not.toEqual({
    dateNow: new Date("07/11/2021 12:00").toISOString(),
    dateYesterday: new Date("07/10/2021 12:00").toISOString(),
    dateNowConvert: new Date("07/11/2021 12:00"),
    dateYesterdayConvert: new Date("07/10/2021 12:00")
  }); 
  expect(CF.isLiveData(false)).not.toEqual({
    dateNow: new Date().toISOString(),
    dateYesterday: new Date(Date.now() - 1 * 24 * 60 * 60 * 1000).toISOString(),
    dateNowConvert: new Date(),
    dateYesterdayConvert: new Date(Date.now() - 1 * 24 * 60 * 60 * 1000)
  }); 
});