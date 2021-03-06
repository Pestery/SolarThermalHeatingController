using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using SunBatherAPI.Data;

namespace SunBatherAPI.Models
{
    // Jack to see in testing
    // Do we need a seperate pump status and set pump, or can they use the same field
    public class SystemStatus
    {
        public int Id { get; set; }
        public int SystemIdentityID { get; set; }
        public bool PumpMode { get; set; }
        public float SetTemperature { get; set; }
        public bool ManualPumpOn { get; set; }
        public bool PumpStatus { get; set; } 
        public SystemIdentity SystemIdentity { get; set; }
    }
}
