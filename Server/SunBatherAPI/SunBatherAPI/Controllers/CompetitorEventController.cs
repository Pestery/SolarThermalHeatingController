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
    [Route("api/CompetitorEvent")]
    [ApiController]
    public class CompetitorEventController : ControllerBase
    {
        private readonly DataContext _context;

        public CompetitorEventController(DataContext context)
        {
            _context = context;
        }

        [HttpGet("{id}")]
        public async Task<ActionResult<IEnumerable<CompetitorEvent>>> GetCompetitorEventList(int id)
        {
            var competitorEvent = await _context.CompetitorEvent.Where(p => p.Id == id).ToListAsync();

            if (competitorEvent == null)
            {
                return NotFound();
            }

            return competitorEvent;
        }

        [Route("[action]/{id}")]
        [HttpGet]
        public async Task<ActionResult<CompetitorEvent>> GetCompetitorEventLatest()
        {
            // orders then finds first id (max), there is another method to do this which is .Max(), but both work fine
            // and are just as fast as each other
            var competitorEvent = await _context.CompetitorEvent.OrderByDescending(p => p.Id).FirstOrDefaultAsync();

            if (competitorEvent == null)
            {
                return NotFound();
            }

            return competitorEvent;
        }

        [Route("[action]/{id}/{dateFrom}/{dateTo}")]
        [HttpGet]
        public async Task<ActionResult<IEnumerable<CompetitorEvent>>> GetCompetitorEventDateToFrom(string dateFrom, string dateTo)
        {
            DateTime dateFromTime = DateTime.Parse(dateFrom);
            DateTime dateToTime = DateTime.Parse(dateTo);

            var competitorEvent = await _context.CompetitorEvent.Where(p => (p.ReadDateTime >= dateFromTime) && (p.ReadDateTime <= dateToTime)).ToListAsync();

            if (competitorEvent == null)
            {
                return NotFound();
            }

            return competitorEvent;
        }
    }
}
