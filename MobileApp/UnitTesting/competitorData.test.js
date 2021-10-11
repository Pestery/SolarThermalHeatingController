const CD = require('../SunBatherApp/app/shared/competitorData');
const compareModel = require("../SunBatherApp/app/models/compareModel");
const competitorModelMod = require("../SunBatherApp/app/models/competitorModel");

test('Test getThermalPower calculator', () => {
  // Initialise Array
  var testArray = [
    { ID: 1, name: "temp1", readDateTime: new Date("07/11/2021 12:00"), temperatureValueInput: 10, temperatureValueOutput: 15 },
    { ID: 2, name: "temp2", readDateTime: new Date("07/11/2021 13:00"), temperatureValueInput: 15.01, temperatureValueOutput: 25.56 },
    { ID: 3, name: "temp3", readDateTime: new Date("07/11/2021 14:00"), temperatureValueInput: 15, temperatureValueOutput: 30 }
  ];

  // (out - in) * 4.184 * 1.7448
  expect(CD.getThermalPower(testArray[0])).toBe(36.501216);
  expect(CD.getThermalPower(testArray[1])).toBe(77.01756576);
  expect(CD.getThermalPower(testArray[2])).toBe(109.503648);
  expect(CD.getThermalPower(testArray[0])).not.toBe(36);
  expect(CD.getThermalPower(testArray[1])).not.toBe(73);
  expect(CD.getThermalPower(testArray[2])).not.toBe(109);
});

// loops through data array
test('Test array sorter function for graphs', () => {
  // Initialise Array
  var testArray = [
    { ID: 1, name: "temp1", readDateTime: new Date("07/11/2021 12:00"), temperatureValueInput: 10, temperatureValueOutput: 11 },
    { ID: 2, name: "temp2", readDateTime: new Date("07/11/2021 13:00"), temperatureValueInput: 15, temperatureValueOutput: 16 },
    { ID: 3, name: "temp3", readDateTime: new Date("07/11/2021 14:00"), temperatureValueInput: 20, temperatureValueOutput: 21.2 },
    { ID: 4, name: "temp4", readDateTime: new Date("07/11/2021 15:00"), temperatureValueInput: 25, temperatureValueOutput: 27 },
    { ID: 5, name: "temp5", readDateTime: new Date("07/11/2021 16:00"), temperatureValueInput: 30, temperatureValueOutput: 29 },
    { ID: 6, name: "temp6", readDateTime: new Date("07/11/2021 17:00"), temperatureValueInput: 35, temperatureValueOutput: 32 }
  ];
  //variables
  var electricityCostpkW = 0.4;   //default cost of 40pc per kW
  //COP's for the 4 kinds of heaters
  const ELECTRICHEATER1 = 5.58 //RTHP026-1

  expect(CD.loopThroughData(testArray, "temperatureValueInput", electricityCostpkW, ELECTRICHEATER1)).toEqual({ 
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureValueInput: 0.5233149247311828, temperatureValueOutput: 11 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureValueInput: 0.5233149247311828, temperatureValueOutput: 16 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureValueInput: 0.627977909677419, temperatureValueOutput: 21.2 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureValueInput: 1.0466298494623656, temperatureValueOutput: 27 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureValueInput: -0.5233149247311828, temperatureValueOutput: 29 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureValueInput: -1.5699447741935482, temperatureValueOutput: 32 }
    ],
    min: -1.5699447741935482,
    max: 1.0466298494623656,
    average: "0.1"
  });

  expect(CD.loopThroughData(testArray, "temperatureValueInput", electricityCostpkW, ELECTRICHEATER1)).not.toEqual({ 
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), temperatureValueInput: 0.5, temperatureValueOutput: 11 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), temperatureValueInput: 0.5, temperatureValueOutput: 16 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), temperatureValueInput: 0.6, temperatureValueOutput: 21.2 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), temperatureValueInput: 1.0, temperatureValueOutput: 27 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), temperatureValueInput: -0.5, temperatureValueOutput: 29 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), temperatureValueInput: -1.5, temperatureValueOutput: 32 }
    ],
    min: -1.5699447741935482,
    max: 1.0466298494623656,
    average: "0.1"
  });
});

test('Test findGraphDataCompare function', () => {
  // Initialise Array
  var testArray = [
    { ID: 1, name: "temp1", readDateTime: new Date("07/11/2021 12:00"), cost: 0.10, emissions: 10, temperatureValueInput: 10, temperatureValueOutput: 11 },
    { ID: 2, name: "temp2", readDateTime: new Date("07/11/2021 13:00"), cost: 0.10, emissions: 10, temperatureValueInput: 15, temperatureValueOutput: 16 },
    { ID: 3, name: "temp3", readDateTime: new Date("07/11/2021 14:00"), cost: 0.10, emissions: 10, temperatureValueInput: 20, temperatureValueOutput: 21.2 },
    { ID: 4, name: "temp4", readDateTime: new Date("07/11/2021 15:00"), cost: 0.10, emissions: 10, temperatureValueInput: 25, temperatureValueOutput: 27 },
    { ID: 5, name: "temp5", readDateTime: new Date("07/11/2021 16:00"), cost: 0.10, emissions: 10, temperatureValueInput: 30, temperatureValueOutput: 29 },
    { ID: 6, name: "temp6", readDateTime: new Date("07/11/2021 17:00"), cost: 0.10, emissions: 10, temperatureValueInput: 35, temperatureValueOutput: 32 }
  ]; 
  var varcompareOptions = compareModel.compareOptions();
  var varCompetitorModelMod = competitorModelMod.competitorOptions();

  expect(CD.findGraphDataCompare(testArray, varcompareOptions[0], varCompetitorModelMod[0])).toEqual({
  graphDataArray: [          
    { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), cost: 0.5233149247311828, emissions: 10, temperatureValueInput: 10, temperatureValueOutput: 11 },
    { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), cost: 0.5233149247311828, emissions: 10, temperatureValueInput: 15, temperatureValueOutput: 16 },
    { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), cost: 0.627977909677419, emissions: 10, temperatureValueInput: 20, temperatureValueOutput: 21.2 },
    { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), cost: 1.0466298494623656, emissions: 10, temperatureValueInput: 25, temperatureValueOutput: 27 },
    { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), cost: -0.5233149247311828, emissions: 10, temperatureValueInput: 30, temperatureValueOutput: 29 },
    { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), cost: -1.5699447741935482, emissions: 10, temperatureValueInput: 35, temperatureValueOutput: 32 }
  ],
  min: "-1.6",
  max: "1.0",
  average: "0.1",
  yAxisInterval: "1"
  });

  expect(CD.findGraphDataCompare(testArray, varcompareOptions[1], varCompetitorModelMod[1])).toEqual({
    graphDataArray: [          
      { ID: 1, name: "temp1", readDateTime: new Date(testArray[0].readDateTime).getTime(), cost: 0.10, emissions: 5.233149247311828, temperatureValueInput: 10, temperatureValueOutput: 11 },
      { ID: 2, name: "temp2", readDateTime: new Date(testArray[1].readDateTime).getTime(), cost: 0.10, emissions: 10, temperatureValueInput: 15, temperatureValueOutput: 16 },
      { ID: 3, name: "temp3", readDateTime: new Date(testArray[2].readDateTime).getTime(), cost: 0.10, emissions: 10, temperatureValueInput: 20, temperatureValueOutput: 21.2 },
      { ID: 4, name: "temp4", readDateTime: new Date(testArray[3].readDateTime).getTime(), cost: 0.10, emissions: 10, temperatureValueInput: 25, temperatureValueOutput: 27 },
      { ID: 5, name: "temp5", readDateTime: new Date(testArray[4].readDateTime).getTime(), cost: 0.10, emissions: 10, temperatureValueInput: 30, temperatureValueOutput: 29 },
      { ID: 6, name: "temp6", readDateTime: new Date(testArray[5].readDateTime).getTime(), cost: 0.10, emissions: 10, temperatureValueInput: 35, temperatureValueOutput: 32 }
    ],
    min: "-3608.1",
    max: "2405.4",
    average: "0.1",
    yAxisInterval: "2005"
    });
});