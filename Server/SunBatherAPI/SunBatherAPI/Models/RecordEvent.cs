using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using SunBatherAPI.Data;

namespace SunBatherAPI.Models
{
    public class RecordEvent
    {
        public int Id { get; set; }
        public Guid SystemIdentityID { get; set; }
        public double TemperatureValueInput { get; set; }
        public double TemperatureValueOutput { get; set; }
        public double TemperatureValueRoof { get; set; }
        public double SolarIrradiance { get; set; }
        public double Cost { get; set; }
        public double Emissions { get; set; }
        public double EnergyAdsorbed { get; set; }
        public DateTime ReadDateTime { get; set; }
        public SystemIdentity SystemIdentity { get; set; }
    }
}
