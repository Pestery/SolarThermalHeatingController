function competitorOptions() {
    var competitorOptionsList = [
      { name: "RTHP026-1", idName: "ELECTRICHEATER1", nameAbbreviated: "RTHP026-1", databaseFieldCost: "gasPumpCost", databaseFieldEmissions: "gasPumpEmissions", isSelected: false },
      { name: "RTHP024-1", idName: "ELECTRICHEATER2", nameAbbreviated: "RTHP024-1", databaseFieldCost: "gasHeaterCost", databaseFieldEmissions: "gasHeaterEmissions", isSelected: false },
      { name: "P0127", idName: "GASHEATER1", nameAbbreviated: "P0127", databaseFieldCost: "gasHeaterCost", databaseFieldEmissions: "gasHeaterEmissions", isSelected: false },
      { name: "127 Premium", idName: "GASHEATER2", nameAbbreviated: "127 Prem", databaseFieldCost: "gasHeaterCost", databaseFieldEmissions: "gasHeaterEmissions", isSelected: false },
    ];
  
    return competitorOptionsList;
}

module.exports = { competitorOptions }