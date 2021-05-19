using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using WebApiPrototype.Data;
using WebApiPrototype.Models;

namespace WebApiPrototype.Controllers
{
    [Route("api/Temperatures")]
    [ApiController]
    public class TemperaturesController : ControllerBase
    {
        private readonly SensorContext _context;

        public TemperaturesController(SensorContext context)
        {
            _context = context;
        }

        // GET: api/Temperatures
        [HttpGet]
        public async Task<ActionResult<IEnumerable<Temperature>>> GetTemperature()
        {
            return await _context.Temperature.ToListAsync();
        }

        // GET: api/Temperatures/5
        [HttpGet("{id}")]
        public async Task<ActionResult<Temperature>> GetTemperature(int id)
        {
            var temperature = await _context.Temperature.FindAsync(id);

            if (temperature == null)
            {
                return NotFound();
            }

            return temperature;
        }

        // PUT: api/Temperatures/5
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPut("{id}")]
        public async Task<IActionResult> PutTemperature(int id, Temperature temperature)
        {
            if (id != temperature.Id)
            {
                return BadRequest();
            }

            _context.Entry(temperature).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!TemperatureExists(id))
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

        // POST: api/Temperatures
        // To protect from overposting attacks, see https://go.microsoft.com/fwlink/?linkid=2123754
        [HttpPost]
        public async Task<ActionResult<Temperature>> PostTemperature(Temperature temperature)
        {
            _context.Temperature.Add(temperature);
            await _context.SaveChangesAsync();

            return CreatedAtAction("GetTemperature", new { id = temperature.Id }, temperature);
        }

        // DELETE: api/Temperatures/5
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteTemperature(int id)
        {
            var temperature = await _context.Temperature.FindAsync(id);
            if (temperature == null)
            {
                return NotFound();
            }

            _context.Temperature.Remove(temperature);
            await _context.SaveChangesAsync();

            return NoContent();
        }

        private bool TemperatureExists(int id)
        {
            return _context.Temperature.Any(e => e.Id == id);
        }
    }
}
