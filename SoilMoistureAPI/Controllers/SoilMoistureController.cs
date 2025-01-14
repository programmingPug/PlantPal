using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using SoilMoistureAPI.Data;
using SoilMoistureAPI.Models;

namespace SoilMoistureAPI.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class SoilMoistureController : ControllerBase
    {
        private readonly SoilMoistureFlatContext _context;

        public SoilMoistureController(SoilMoistureFlatContext context)
        {
            _context = context;
        }

        // POST: api/SoilMoisture
        [HttpPost]
        public async Task<IActionResult> PostSoilMoisture([FromBody] SoilMoistureDto data)
        {
            if (data == null)
            {
                return BadRequest("No data received.");
            }

            if (string.IsNullOrEmpty(data.DeviceId))
            {
                ModelState.AddModelError("DeviceId", "The DeviceId field is required.");
            }
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            // Attempt to find an existing record for the device
            var existingRecord = await _context.SoilMoisturesFlat
                .FirstOrDefaultAsync(sm => sm.DeviceId == data.DeviceId);

            if (existingRecord != null)
            {
                // Update existing record
                existingRecord.MoistureLevel = data.MoistureLevel;
                existingRecord.Timestamp = DateTime.UtcNow;

                _context.SoilMoisturesFlat.Update(existingRecord);
                await _context.SaveChangesAsync();
                return Ok(existingRecord);
            }
            else
            {
                // Insert new record
                var newRecord = new SoilMoistureFlat
                {
                    DeviceId = data.DeviceId,
                    MoistureLevel = data.MoistureLevel,
                    Timestamp = DateTime.UtcNow,
                    Nickname = "Dave"
                };

                _context.SoilMoisturesFlat.Add(newRecord);
                await _context.SaveChangesAsync();
                return Ok(newRecord);
            }
        }
    

        // GET: api/SoilMoisture/5
        [HttpGet("{id}")]
        public async Task<ActionResult<SoilMoistureFlat>> GetSoilMoisture(int id)
        {
            var soilMoisture = await _context.SoilMoisturesFlat.FindAsync(id);

            if (soilMoisture == null)
            {
                return NotFound();
            }

            return soilMoisture;
        }

        // GET: api/SoilMoisture
        [HttpGet]
        public async Task<ActionResult<IEnumerable<SoilMoistureFlat>>> GetSoilMoistures()
        {
            return await _context.SoilMoisturesFlat.ToListAsync();
        }
    }
}