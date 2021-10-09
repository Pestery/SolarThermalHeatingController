using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using SunBatherAPI.Data;
using SunBatherAPI.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace SunBatherAPI.Controllers
{
    /// <summary>
    /// Used to manage communication between web-API and controller/Arduino
    /// </summary>
    [Route("api/System")]
    [ApiController]
    public class SystemController : ControllerBase
    {
        private readonly DataContext _context;

        public SystemController(DataContext context)
        {
            _context = context;
            //System.Diagnostics.Debug.WriteLine("SystemController() constructor");
        }

        // POST: api/TemperatureSensors
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public async Task<ActionResult<ControllerReply>> Post(ControllerPost param)
        {
            var temp = await _context.SystemID.FirstOrDefaultAsync(p => p.Id == 1);
            System.Diagnostics.Debug.WriteLine("GUID: " + temp.ProductId);

            // Make sure that something was posted
            if (param == null) return NoContent();

            // Verify GUID of controller
            // Also get the system identifier data
            //System.Diagnostics.Debug.WriteLine("SystemController: Request from GUID: " + param.Guid);
            var systemId = await _context.SystemID.FirstOrDefaultAsync(p => p.ProductId == param.Guid);
            if ((systemId == null) || (systemId.ProductId != param.Guid))
            {
                //System.Diagnostics.Debug.WriteLine("SystemController: GUID not found: " + param.Guid);
                return NoContent();
            }

            // Get system status, via a task
            // It will be needed later and can be started now
            var systemStatusTask = _context.SystemStatus.FirstOrDefaultAsync(i => i.Id == systemId.Id);

            // Check if new record events data were provided
            // If so then add all to the database
            if (param.Records != null)
            {
                foreach (var record in param.Records)
                {
                    // Convert the received unix time to a DateTime value
                    DateTime dt = DateTime.SpecifyKind(DateTimeOffset.FromUnixTimeSeconds(record.Date).DateTime, DateTimeKind.Utc);

                    // Check if the record already exists within the database
                    if (!_context.RecordEvent.Where(p => (p.SystemIdentity == systemId) && (p.ReadDateTime == dt)).Any()) {

                        // Add the new record
                        _context.RecordEvent.Add(
                            new RecordEvent()
                            {
                                SystemIdentityID = systemId.Id,
                                TemperatureValueInput = record.Tin,
                                TemperatureValueOutput = record.Tout,
                                TemperatureValueRoof = record.Troof,
                                SolarIrradiance = record.Solar,
                                Cost = 0,
                                Emissions = 0,
                                EnergyAdsorbed = 0,
                                ReadDateTime = dt,
                                SystemIdentity = systemId
                            }
                        );
                    }
                }
            }

            // Get a reference to the current controller settings and status data
            //var systemStatus = _context.SystemStatus.FirstOrDefault(i => i.Id == systemId.Id);
            var systemStatus = await systemStatusTask;

            // Check if controller status data was provided
            // If so then update the database accordingly
            if (param.Status != null)
            {
                // Check for pump status
                if (param.Status.PumpOn != null)
                {
                    systemStatus.PumpStatus = (bool)param.Status.PumpOn;
                }
            }

            // Save all changes to the database
            // After this point, all database access is read-only
            _context.SaveChanges();

            // Get a set of all records which belong to this controller
            var systemRecords = _context.RecordEvent.Where(p => p.SystemIdentity == systemId);
            
            // Get the unix time of the most recent record
            // If no records exist then use a value of zero instead
            long lastRecordReceived =
                systemRecords.Any() ?
                ((DateTimeOffset)DateTime.SpecifyKind(systemRecords.Max(p => p.ReadDateTime), DateTimeKind.Utc)).ToUnixTimeSeconds() :
                0;

            // Generate a reply which can be sent to the controller
            ControllerReply reply = new()
            {
                Auto = systemStatus.PumpMode,
                SetTemp = systemStatus.SetTemperature,
                PumpOn = systemStatus.ManualPumpOn,
                LastRecord = lastRecordReceived
            };

            // Send reply to controller
            return reply;
        }
    }
}
