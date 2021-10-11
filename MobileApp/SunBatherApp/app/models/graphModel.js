function graphOptions() {
  var graphOptionsList = [
    { name: "Temperature Value In", nameAbbreviated: "Temp In", databaseField: "temperatureValueInput", isSelected: false, },
    { name: "Temperature Value Out", nameAbbreviated: "Temp Out", databaseField: "temperatureValueOutput", isSelected: false },
    { name: "Temperature Value Roof", nameAbbreviated: "Temp Roof", databaseField: "temperatureValueRoof", isSelected: false },
    { name: "Solar Irradiance", nameAbbreviated: "Solar Energy", databaseField: "solarIrradiance", isSelected: false },
    //{ name: "Cost", nameAbbreviated: "Cost", databaseField: "cost", isSelected: false },
    //{ name: "Emissions", nameAbbreviated: "Emissions", databaseField: "emissions", isSelected: false },
    { name: "Energy Adsorbed", nameAbbreviated: "Energy", databaseField: "energyAdsorbed", isSelected: false },
  ];

  return graphOptionsList;
}

module.exports = { graphOptions }