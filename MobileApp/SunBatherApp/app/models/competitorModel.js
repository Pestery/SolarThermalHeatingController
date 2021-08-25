export function competitorOptions() {
    var competitorOptionsList = [
      { name: "Gas Heater", nameAbbreviated: "Heater", databaseFieldCost: "gasPumpCost", databaseFieldEmissions: "gasPumpEmissions", costConstant: 1.5, emissionConstant: 2 ,isSelected: false },
      { name: "Gas Pump", nameAbbreviated: "Pump", databaseFieldCost: "gasHeaterCost", databaseFieldEmissions: "gasHeaterEmissions", costConstant: 2, emissionConstant: 3, isSelected: false },
    ];
  
    return competitorOptionsList;
}