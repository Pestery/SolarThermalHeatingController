export function competitorOptions() {
    var competitorOptionsList = [
      { name: "Gas Heater", nameAbbreviated: "Heater", databaseFieldCost: "gasPumpCost", databaseFieldEmissions: "gasPumpEmissions", isSelected: false },
      { name: "Gas Pump", nameAbbreviated: "Pump", databaseFieldCost: "gasHeaterCost", databaseFieldEmissions: "gasHeaterEmissions", isSelected: false },
    ];
  
    return competitorOptionsList;
}