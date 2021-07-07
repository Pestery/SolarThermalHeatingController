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
    [Route("api/SystemStatus")]
    [ApiController]
    public class SystemStatusController : ControllerBase
    {
        private readonly DataContext _context;

        public SystemStatusController(DataContext context)
        {
            _context = context;
        }

        [HttpGet("{id}")]
        public async Task<ActionResult<SystemStatus>> GetSystemStatus(int? id)
        {
            var systemStatus = await _context.SystemStatus.FirstOrDefaultAsync(p => p.SystemIdentityID == id);

            if (systemStatus == null)
            {
                return NotFound();
            }

            return systemStatus;
        }

        [HttpPut("{id}")]
        public async Task<IActionResult> PutSystemStatus(int id, SystemStatus systemStatus)
        {
            if (id != systemStatus.Id)
            {
                return BadRequest();
            }

            _context.Entry(systemStatus).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!SystemStatusExists(id))
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

        private bool SystemStatusExists(int id)
        {
            return _context.SystemStatus.Any(e => e.Id == id);
        }
    }
}
