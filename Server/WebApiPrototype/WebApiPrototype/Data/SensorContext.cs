using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using WebApiPrototype.Models;

namespace WebApiPrototype.Data
{
      public class SensorContext : DbContext
      {
            public SensorContext(DbContextOptions<SensorContext> options) : base(options)
            {
            }

            public DbSet<TemperatureSensor> TemperatureSensor { get; set; }
      }
   
}
