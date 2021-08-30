using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using SunBatherAPI.Data;
using SunBatherAPI.Models;

namespace SunBatherAPI.Controllers
{
    [Route("api/TemperatureSensors")]
    [ApiController]
    public class TemperatureSensorsController : ControllerBase
    {
        private readonly DataContext _context;

        public TemperatureSensorsController(DataContext context)
        {
            _context = context;
        }

        // GET: api/TemperatureSensors
        [HttpGet]
        public async Task<ActionResult<IEnumerable<TemperatureSensor>>> GetTemperatureSensor()
        {
            return await _context.TemperatureSensor.ToListAsync();
        }

        // GET: api/TemperatureSensors/5
        [HttpGet("{id}")]
        public async Task<ActionResult<TemperatureSensor>> GetTemperatureSensor(int id)
        {
            var temperatureSensor = await _context.TemperatureSensor.FindAsync(id);

            if (temperatureSensor == null)
            {
                return NotFound();
            }

            return temperatureSensor;
        }

        // Added by jack, used for prototype demo, just grabs the latest temp value from db for phone to display
        // is called by http://localhost:5000/api/TemperatureSensors/GetLatest
        [Route("[action]")]
        [HttpGet]
        public async Task<ActionResult<TemperatureSensor>> GetLatest()
        {
            // orders then finds first id (max), there is another method to do this which is .Max(), but both work fine
            // and are just as fast as each other
            var temperatureSensor = await _context.TemperatureSensor.OrderByDescending(p => p.Id).FirstOrDefaultAsync();

            if (temperatureSensor == null)
            {
                return NotFound();
            }

            return temperatureSensor;
        }

        // PUT: api/TemperatureSensors/5
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPut("{id}")]
        public async Task<IActionResult> PutTemperatureSensor(int id, TemperatureSensor temperatureSensor)
        {
            if (id != temperatureSensor.Id)
            {
                return BadRequest();
            }

            _context.Entry(temperatureSensor).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!TemperatureSensorExists(id))
                {
                    return NotFound();
                }
                else
                {
                    throw;
                }
            }

            return NoContent();
        }

        // POST: api/TemperatureSensors
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public async Task<ActionResult<TemperatureSensor>> PostTemperatureSensor(TemperatureSensor temperatureSensor)
        {
            _context.TemperatureSensor.Add(temperatureSensor);
            await _context.SaveChangesAsync();

            return CreatedAtAction("GetTemperatureSensor", new { id = temperatureSensor.Id }, temperatureSensor);
        }

        // DELETE: api/TemperatureSensors/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteTemperatureSensor(int id)
        {
            var temperatureSensor = await _context.TemperatureSensor.FindAsync(id);
            if (temperatureSensor == null)
            {
                return NotFound();
            }

            _context.TemperatureSensor.Remove(temperatureSensor);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool TemperatureSensorExists(int id)
        {
            return _context.TemperatureSensor.Any(e => e.Id == id);
        }
    }
}
