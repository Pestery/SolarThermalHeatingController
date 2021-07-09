using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using SunBatherAPI.Models;

namespace SunBatherAPI.Data
{
    public static class DBInitializer
    {
        public static void Initialize(DataContext context)
        {
            context.Database.EnsureCreated();

            Guid[] GuidsArray;
            GuidsArray = GenerateGuidArray();

            if (context.TemperatureSensor.Any())
            {
                return; // DB has already been seeded
            }

            var temperature = new TemperatureSensor[]{
                new TemperatureSensor{Id=1,Value=19.21,ReadDate=DateTime.Parse("01/09/2005 12:10")},
                new TemperatureSensor{Id=2,Value=20.27,ReadDate=DateTime.Parse("01/09/2005 12:20")},
                new TemperatureSensor{Id=3,Value=21.12,ReadDate=DateTime.Parse("01/09/2005 12:30")},
                new TemperatureSensor{Id=4,Value=21.05,ReadDate=DateTime.Parse("01/09/2005 12:40")},
                new TemperatureSensor{Id=5,Value=21.02,ReadDate=DateTime.Parse("01/09/2005 12:50")}
            };
            foreach (TemperatureSensor t in temperature)
            {
                context.TemperatureSensor.Add(t);
            }
            context.SaveChanges();

            var systemID = new SystemIdentity[]{
                new SystemIdentity{Id=1,ProductId=GuidsArray[0]},
                new SystemIdentity{Id=2,ProductId=GuidsArray[1]},
                new SystemIdentity{Id=3,ProductId=GuidsArray[2]}
            };
            foreach (SystemIdentity sID in systemID)
            {
                context.SystemID.Add(sID);
            }
            context.SaveChanges();

            var recordEvent = new RecordEvent[]{
                new RecordEvent{Id=1,SystemIdentityID=1,TemperatureValueInput=15.11,TemperatureValueOutput=19.20,TemperatureValueRoof=30.79,SolarIrradiance=1010,Emissions=100,Cost=0.25,EnergyAdsorbed=700,ReadDateTime=DateTime.Parse("2005-09-01 12:10")},
                new RecordEvent{Id=2,SystemIdentityID=1,TemperatureValueInput=15.49,TemperatureValueOutput=20.21,TemperatureValueRoof=30.23,SolarIrradiance=1021,Emissions=100,Cost=0.25,EnergyAdsorbed=723,ReadDateTime=DateTime.Parse("2005-09-01 12:20")},
                new RecordEvent{Id=3,SystemIdentityID=1,TemperatureValueInput=17.12,TemperatureValueOutput=22.22,TemperatureValueRoof=31.22,SolarIrradiance=1102,Emissions=50,Cost=0.12,EnergyAdsorbed=550,ReadDateTime=DateTime.Parse("2005-09-01 12:30")},
                new RecordEvent{Id=4,SystemIdentityID=1,TemperatureValueInput=19.33,TemperatureValueOutput=24.43,TemperatureValueRoof=32.71,SolarIrradiance=1107,Emissions=100,Cost=0.25,EnergyAdsorbed=751,ReadDateTime=DateTime.Parse("2005-09-01 12:40")},
                new RecordEvent{Id=5,SystemIdentityID=1,TemperatureValueInput=21.47,TemperatureValueOutput=25.67,TemperatureValueRoof=32.73,SolarIrradiance=1101,Emissions=100,Cost=0.25,EnergyAdsorbed=749,ReadDateTime=DateTime.Parse("2005-09-01 12:50")}
            };
            foreach (RecordEvent re in recordEvent)
            {
                context.RecordEvent.Add(re);
            }
            context.SaveChanges();

            // pump true/false auto/manual on/off
            var systemStatus = new SystemStatus[]{
                new SystemStatus{Id=1,SystemIdentityID=1,PumpMode=true,SetTemperature=27,PumpStatus=true},
                new SystemStatus{Id=2,SystemIdentityID=2,PumpMode=true,SetTemperature=22,PumpStatus=false},
                new SystemStatus{Id=3,SystemIdentityID=3,PumpMode=true,SetTemperature=20,PumpStatus=false}
            };
            foreach (SystemStatus ss in systemStatus)
            {
                context.SystemStatus.Add(ss);
            }
            context.SaveChanges();
        }

        // generates guids
        public static Guid[] GenerateGuidArray()
        {
            int NumberOfGuids = 5;
            Guid[] GuidArray = new Guid[NumberOfGuids];
            for (int i = 0; i < NumberOfGuids; i++){
                GuidArray[i] = Guid.NewGuid();
            }

            return GuidArray;
        }
    }
}
