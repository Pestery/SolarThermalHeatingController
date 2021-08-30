using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;
using SunBatherAPI.Models;

namespace SunBatherAPI.Data
{
      public class DataContext : DbContext
      {
            public DataContext(DbContextOptions<DataContext> options) : base(options)
            {
            }

            public DbSet<TemperatureSensor> TemperatureSensor { get; set; }
            public DbSet<SystemIdentity> SystemID { get; set; }
            public DbSet<SystemStatus> SystemStatus { get; set; }
            public DbSet<RecordEvent> RecordEvent { get; set; }
      }
}
