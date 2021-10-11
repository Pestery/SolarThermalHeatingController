function compareOptions() {
  var compareOptionsList = [
    { name: "Cost", nameAbbreviated: "Cost", databaseField: "cost", isSelected: false },
    { name: "Emissions", nameAbbreviated: "Emissions", databaseField: "emissions", isSelected: false },
  ];

  return compareOptionsList;
}

module.exports = { compareOptions }
