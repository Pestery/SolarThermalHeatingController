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
            // Make sure that something was posted
            if (param == null) return NoContent();

            //var temp = await _context.SystemID.FirstOrDefaultAsync(p => p.Id == 3);
            //System.Diagnostics.Debug.WriteLine("GUID: " + temp.ProductId);

            // Verify GUID of controller
            // Also get the system identifier data
            //System.Diagnostics.Debug.WriteLine("SystemController: Request from GUID: " + param.Guid);
            var systemId = await _context.SystemID.FirstOrDefaultAsync(p => p.ProductId == param.Guid);
            if ((systemId == null) || (systemId.ProductId != param.Guid))
            {
                //System.Diagnostics.Debug.WriteLine("SystemController: GUID not found: " + param.Guid);
                return NoContent();
            }

            // Get a reference to the current controller settings and status data
            var systemStatus = _context.SystemStatus.FirstOrDefault(i => i.Id == systemId.Id);

            // Check if new record events data were provided
            // If so then add all to the database
            if (param.Records != null)
            {
                foreach (var record in param.Records)
                {
                    _context.RecordEvent.Add(
                        new RecordEvent()
                        {
                            //Id = record.Id,
                            SystemIdentityID = systemId.Id,
                            TemperatureValueInput = record.Tin,
                            TemperatureValueOutput = record.Tout,
                            TemperatureValueRoof = record.Troof,
                            SolarIrradiance = 0,
                            Cost = 0,
                            Emissions = 0,
                            EnergyAdsorbed = 0,
                            ReadDateTime = DateTimeOffset.FromUnixTimeSeconds(record.Date).DateTime,
                            SystemIdentity = systemId
                        }
                    );
                }
            }

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

            // Generate a reply which can be sent to the controller
            ControllerReply reply = new()
            {
                Auto = systemStatus.PumpMode,
                SetTemp = systemStatus.SetTemperature,
                PumpOn = systemStatus.ManualPumpOn
            };

            // Save all changes to the database
            await _context.SaveChangesAsync();

            // Send reply to controller
            return reply;
        }
    }
}
