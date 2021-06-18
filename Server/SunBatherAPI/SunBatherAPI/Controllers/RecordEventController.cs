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
        public async Task<ActionResult<IEnumerable<RecordEvent>>> GetRecordEventList(Guid id)
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
        public async Task<ActionResult<RecordEvent>> GetRecordEventLatest(Guid id)
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
    }
}
