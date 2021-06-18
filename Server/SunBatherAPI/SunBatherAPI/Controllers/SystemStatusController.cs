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
        public async Task<ActionResult<SystemStatus>> GetSystemStatus(Guid? id)
        {
            var test = await _context.SystemStatus.FirstOrDefaultAsync();
            var systemStatus = await _context.SystemStatus.FirstOrDefaultAsync(p => p.SystemIdentityID == id);

            if (systemStatus == null)
            {
                return NotFound();
            }

            return systemStatus;
        }

    }
}
