using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using SunBatherAPI.Data;
using SunBatherAPI.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.EntityFrameworkCore;

namespace SunBatherAPI.Controllers
{
    [Route("api/RecordEvent")]
    [ApiController]
    public class RecordEventController : ControllerBase
    {
        private readonly DataContext _context;

        public RecordEventController(DataContext context)
        {
            _context = context;
        }

        [HttpGet("{id}")]
        public async Task<ActionResult<IEnumerable<RecordEvent>>> GetRecordEventList(int id)
        {
            var recordEvent = await _context.RecordEvent.Where(p => p.SystemIdentityID == id).ToListAsync();

            if (recordEvent == null)
            {
                return NotFound();
            }

            return recordEvent;
        }

        [Route("[action]/{id}")]
        [HttpGet]
        public async Task<ActionResult<RecordEvent>> GetRecordEventLatest(int id)
        {
            // orders then finds first id (max), there is another method to do this which is .Max(), but both work fine
            // and are just as fast as each other
            var recordEvent = await _context.RecordEvent.OrderByDescending(p => p.Id).FirstOrDefaultAsync(p => p.SystemIdentityID == id);

            if (recordEvent == null)
            {
                return NotFound();
            }

            return recordEvent;
        }

        [Route("[action]/{id}/{dateFrom}/{dateTo}")]
        [HttpGet]
        public async Task<ActionResult<IEnumerable<RecordEvent>>> GetRecordEventDateToFrom(int id, string dateFrom, string dateTo)
        {
            DateTime dateFromTime = DateTime.Parse(dateFrom);
            DateTime dateToTime = DateTime.Parse(dateTo);

            // Get a set containing all records within the date-time range
            var records = _context.RecordEvent.Where(p => (p.SystemIdentityID == id) && (p.ReadDateTime >= dateFromTime) && (p.ReadDateTime <= dateToTime));
            if (records == null) return NotFound();

            // Get the total number of records found
            // Also set a rough maximum number which should be returned
            int totalRecords = await records.CountAsync();
            int maxRecords = 1000;
            if (totalRecords > maxRecords)
            {
                // Some of the records will be averaged together to reduce the total number returned
                // Calculate how many should be averaged together
                int stepAmount = totalRecords / maxRecords;
                if (stepAmount > 1)
                {
                    // Create a result list to which the records will be added
                    // Also create a working variable to use to sum records together
                    List<RecordEvent> result = new();
                    RecordEvent first = await records.FirstAsync();
                    RecordEvent temp = new()
                    {
                        //Id = first.Id,
                        SystemIdentityID = first.SystemIdentityID,
                        SystemIdentity = first.SystemIdentity,
                        TemperatureValueInput = 0,
                        TemperatureValueOutput = 0,
                        TemperatureValueRoof = 0,
                        SolarIrradiance = 0,
                        Cost = 0,
                        Emissions = 0,
                        EnergyAdsorbed = 0
                    };

                    // Loop through all the records found and average them together as needed
                    int count = 0;
                    foreach(RecordEvent r in records)
                    {
                        // Check if the target number of recordds have been averaged together
                        // If so then add the result to the result list
                        if (count >= stepAmount)
                        {
                            // Divide the record values by the total count
                            // This will convert the sum value into an average value
                            temp.TemperatureValueInput /= count;
                            temp.TemperatureValueOutput /= count;
                            temp.TemperatureValueRoof /= count;
                            temp.SolarIrradiance /= count;
                            temp.Cost /= count;
                            temp.Emissions /= count;
                            temp.EnergyAdsorbed /= count;

                            // Add this record to the result set
                            result.Add(temp);

                            // Reset values for next step
                            count = 0;
                            temp = new()
                            {
                                //Id = first.Id,
                                SystemIdentityID = first.SystemIdentityID,
                                SystemIdentity = first.SystemIdentity,
                                TemperatureValueInput = 0,
                                TemperatureValueOutput = 0,
                                TemperatureValueRoof = 0,
                                SolarIrradiance = 0,
                                Cost = 0,
                                Emissions = 0,
                                EnergyAdsorbed = 0
                            };
                        }

                        // Add this record to the working total
                        temp.TemperatureValueInput += r.TemperatureValueInput;
                        temp.TemperatureValueOutput += r.TemperatureValueOutput;
                        temp.TemperatureValueRoof += r.TemperatureValueRoof;
                        temp.SolarIrradiance += r.SolarIrradiance;
                        temp.Cost += r.Cost;
                        temp.Emissions += r.Emissions;
                        temp.EnergyAdsorbed += r.EnergyAdsorbed;

                        // Set the date-time and increment the counter
                        temp.ReadDateTime = r.ReadDateTime;
                        count++;
                    }

                    // Finish up
                    // Divide the record values by the total count, if required
                    if (count > 2)
                    {
                        temp.TemperatureValueInput /= count;
                        temp.TemperatureValueOutput /= count;
                        temp.TemperatureValueRoof /= count;
                        temp.SolarIrradiance /= count;
                        temp.Cost /= count;
                        temp.Emissions /= count;
                        temp.EnergyAdsorbed /= count;
                    }

                    // Add this record to the result set, if required
                    // Return result
                    if (count > 0) result.Add(temp);
                    return result;
                }
            }

            // Just return everything
            return await records.ToListAsync();
        }
    }
}
