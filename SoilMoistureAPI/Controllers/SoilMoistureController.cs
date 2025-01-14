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
        private readonly SoilMoistureContext _context;

        public SoilMoistureController(SoilMoistureContext context)
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

            // Basic validation: ensure required fields are provided.
            if (string.IsNullOrEmpty(data.DeviceId))
            {
                ModelState.AddModelError("DeviceId", "The DeviceId field is required.");
            }
            // You might also want to validate moisture level here.
            if (!ModelState.IsValid)
            {
                return BadRequest(ModelState);
            }

            // Check if a record for this device already exists
            var existingRecord = await _context.SoilMoistures
                .Include(sm => sm.Device)
                .FirstOrDefaultAsync(sm => sm.DeviceId == data.DeviceId);

            if (existingRecord != null)
            {
                // Update the existing record
                existingRecord.MoistureLevel = data.MoistureLevel;
                existingRecord.Timestamp = DateTime.UtcNow;

                _context.SoilMoistures.Update(existingRecord);
                await _context.SaveChangesAsync();
                return Ok(existingRecord);
            }
            else
            {
                // Create a new record
                var newRecord = new SoilMoisture
                {
                    DeviceId = data.DeviceId,
                    MoistureLevel = data.MoistureLevel,
                    Timestamp = DateTime.UtcNow,
                    Device = new Device
                    {
                        DeviceId = data.DeviceId,
                        Nickname = "Dave"
                    }
                };

                _context.SoilMoistures.Add(newRecord);
                await _context.SaveChangesAsync();
                return Ok(newRecord);
            }
        }

        // GET: api/SoilMoisture/5
        [HttpGet("{id}")]
        public async Task<ActionResult<SoilMoisture>> GetSoilMoisture(int id)
        {
            var soilMoisture = await _context.SoilMoistures.FindAsync(id);

            if (soilMoisture == null)
            {
                return NotFound();
            }

            return soilMoisture;
        }

        // GET: api/SoilMoisture
        [HttpGet]
        public async Task<ActionResult<IEnumerable<SoilMoisture>>> GetSoilMoistures()
        {
            return await _context.SoilMoistures.ToListAsync();
        }
    }
}