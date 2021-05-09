using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApiPrototype.Data;

namespace WebApiPrototype.Models
{
    public class TemperatureSensor
    {
        public int Id { get; set; }
        public double Value { get; set; }
        public DateTime ReadDate { get; set; }
    }
}
