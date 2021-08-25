using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace SunBatherAPI.Models
{
    public class CompetitorEvent
    {
        public int Id { get; set; }
        public double GasPumpCost { get; set; }
        public double GasHeaterCost { get; set; }
        public double GasPumpEmissions { get; set; }
        public double GasHeaterEmissions { get; set; }
        public DateTime ReadDateTime { get; set; }
    }
}
