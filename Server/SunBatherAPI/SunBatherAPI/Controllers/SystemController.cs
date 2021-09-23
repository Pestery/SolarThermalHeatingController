using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
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
            System.Diagnostics.Debug.WriteLine("SystemController() constructor");
        }

        // POST: api/TemperatureSensors
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public ActionResult<ControllerReply> Post(ControllerPost param)
        {
            //System.Diagnostics.Debug.WriteLine("Post request");

            //_context.TemperatureSensor.Add(in);
            //await _context.SaveChangesAsync();

            ControllerReply reply = new()
            {
                Auto = false,
                SetTemp = 22
            };

            return reply;
        }
    }
}
