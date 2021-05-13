using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using WebApiPrototype.Models;

namespace WebApiPrototype.Data
{
    public static class DBInitializer
    {
        public static void Initialize(SensorContext context)
        {
            context.Database.EnsureCreated();

            if (context.TemperatureSensor.Any()){
                return; // DB has already been seeded
            }

            var temperature = new TemperatureSensor[]{
                new TemperatureSensor{Id=1,Value=19.21, ReadDate=DateTime.Parse("2005-09-01 12:10")},
                new TemperatureSensor{Id=2,Value=20.27, ReadDate=DateTime.Parse("2005-09-01 12:20")},
                new TemperatureSensor{Id=3,Value=21.12, ReadDate=DateTime.Parse("2005-09-01 12:30")},
                new TemperatureSensor{Id=4,Value=21.05, ReadDate=DateTime.Parse("2005-09-01 12:40")},
                new TemperatureSensor{Id=5,Value=21.02, ReadDate=DateTime.Parse("2005-09-01 12:50")}
            };
            foreach(TemperatureSensor t in temperature){
                context.TemperatureSensor.Add(t);
            }
            context.SaveChanges();
        }
    }
}
