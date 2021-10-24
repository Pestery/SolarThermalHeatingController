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

            if (context.TemperatureSensor.Any())
            {
                return; // DB has already been seeded
            }
            Console.WriteLine("SeedingData");

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

            Guid[] GuidsArray;
            GuidsArray = GenerateGuidArray();

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

            RecordEvent[] recordEvent;
            int numberOfRecords = 200;
            int timeInterval = 15; // in minutes 
            bool dateTimeNow = false;
            DateTime testingTime;

            // can go to live times or a set time, set time is useful for testing on the mobile app, production will have live
            if (dateTimeNow) {
                testingTime = DateTime.Now;
            } else {
                testingTime = DateTime.Parse("11/07/2021 12:00");
            }

            recordEvent = RecordEventArrayGenerator(numberOfRecords, timeInterval, testingTime);
            foreach (RecordEvent re in recordEvent)
            {
                context.RecordEvent.Add(re);
            }
            context.SaveChanges();

            // pump true/false auto/manual on/off
            var systemStatus = new SystemStatus[]{
                new SystemStatus{Id=1,SystemIdentityID=1,PumpMode=true,SetTemperature=27,ManualPumpOn=true,PumpStatus=true},
                new SystemStatus{Id=2,SystemIdentityID=2,PumpMode=true,SetTemperature=22,ManualPumpOn=true,PumpStatus=false},
                new SystemStatus{Id=3,SystemIdentityID=3,PumpMode=true,SetTemperature=20,ManualPumpOn=false,PumpStatus=false}
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

        // cost cant be negative so this is used to prevent it, example max cost of running system is $0.5
        public static double CostData(double cost, double minimum, double maximum, Random rnd)
        {
            cost = Math.Round(cost + (rnd.NextDouble() * (maximum - minimum) + minimum), 1);

            if (cost < 0) {
                cost = 0;
            } else if (cost > 0.5) {
                cost = 0.5;
            }

            return cost; 
        }

        // Emissions cant be negative so this is used to prevent it, example max Emission of running system is 1
        public static double EmissionData(double emission, double minimum, double maximum, Random rnd)
        {
            emission = Math.Round(emission + (rnd.NextDouble() * (maximum - minimum) + minimum), 1);

            if (emission < 0) {
                emission = 0;
            }
            else if (emission > 1) {
                emission = 1;
            }

            return emission;
        }

        // generates data (only for one customer)
        // functions works by counting back from the current time NOW, in intervals of the time you set (eg. 20 minutes)
        public static RecordEvent[] RecordEventArrayGenerator(int samplesGenerated, int timeInterval, DateTime dateTime)
        {
            // initialise values
            RecordEvent[] RecordEventArray = new RecordEvent[samplesGenerated];
            Random rnd = new Random();
            double maximum = 1;
            double minimum = -1;
            double temperatureInChange = 15;
            double temperatureOutChange = 20;
            double temperatureRoofChange = 30;
            double solarIrradianceChange = 1000;
            double emissions = 1;
            double cost = 0.25;
            double energyAdsorbed = 700;

            for (int i = samplesGenerated; i > 0; i--) {
                RecordEventArray[i - 1] = new RecordEvent{ 
                    Id = i, 
                    SystemIdentityID = 1, 
                    TemperatureValueInput = temperatureInChange, 
                    TemperatureValueOutput = temperatureOutChange, 
                    TemperatureValueRoof = temperatureRoofChange, 
                    SolarIrradiance = solarIrradianceChange, 
                    Emissions = emissions, 
                    Cost = cost, 
                    EnergyAdsorbed = energyAdsorbed, 
                    ReadDateTime = dateTime };

                temperatureInChange = Math.Round(temperatureInChange + (rnd.NextDouble() * (maximum - minimum) + minimum), 1);
                temperatureOutChange = Math.Round(temperatureOutChange + (rnd.NextDouble() * (maximum - minimum) + minimum), 1);
                temperatureRoofChange = Math.Round(temperatureRoofChange + (rnd.NextDouble() * (maximum - minimum) + minimum), 1);
                solarIrradianceChange = Math.Round(solarIrradianceChange + (rnd.NextDouble() * ((maximum * 10) - (minimum * 10)) + (minimum * 10)), 1);
                emissions = EmissionData(cost, minimum, maximum, rnd);
                cost = CostData(cost, minimum, maximum, rnd);
                energyAdsorbed = Math.Round(energyAdsorbed + (rnd.NextDouble() * ((maximum * 10 ) - (minimum * 10)) + (minimum * 10)), 1);
                dateTime = dateTime.AddMinutes(-timeInterval);
            }

            return RecordEventArray;
        }
    }
}
