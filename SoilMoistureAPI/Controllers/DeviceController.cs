using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using SoilMoistureAPI.Data;
using SoilMoistureAPI.Models;
using System.Threading.Tasks;

namespace SoilMoistureAPI.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class DeviceController : ControllerBase
    {
        private readonly SoilMoistureFlatContext _context;

        public DeviceController(SoilMoistureFlatContext context)
        {
            _context = context;
        }

        // POST: api/Device
        [HttpPost]
        public async Task<ActionResult<Device>> CreateDevice(SoilMoistureFlat device)
        {
            if (device == null || string.IsNullOrEmpty(device.DeviceId))
            {
                return BadRequest("Invalid device data.");
            }

            // Check if device already exists
            var existingDevice = await _context.SoilMoisturesFlat.FindAsync(device.DeviceId);
            if (existingDevice != null)
            {
                return Conflict("Device with the same DeviceId already exists.");
            }

            _context.SoilMoisturesFlat.Add(device);
            await _context.SaveChangesAsync();

            return CreatedAtAction(nameof(GetDevice), new { id = device.DeviceId }, device);
        }

        // GET: api/Device/{id}
        [HttpGet("{id}")]
        public async Task<ActionResult<SoilMoistureFlat>> GetDevice(string id)
        {
            var device = await _context.SoilMoisturesFlat
                .FirstOrDefaultAsync(d => d.DeviceId == id);

            if (device == null)
            {
                return NotFound();
            }

            return device;
        }

        // PUT: api/Device/{id}/nickname
        [HttpPut("{id}/nickname")]
        public async Task<IActionResult> UpdateNickname(string id, [FromBody] string newNickname)
        {
            if (string.IsNullOrEmpty(newNickname))
            {
                return BadRequest("Nickname cannot be empty.");
            }

            var device = await _context.SoilMoisturesFlat.FindAsync(id);
            if (device == null)
            {
                return NotFound("Device not found.");
            }

            device.Nickname = newNickname;
            _context.Entry(device).State = EntityState.Modified;

            try
            {
                await _context.SaveChangesAsync();
                return NoContent(); // 204 No Content
            }
            catch (DbUpdateConcurrencyException)
            {
                if (!DeviceExists(id))
                {
                    return NotFound("Device not found.");
                }
                else
                {
                    throw;
                }
            }
        }

        // DELETE: api/Device/{id}
        [HttpDelete("{id}")]
        public async Task<IActionResult> DeleteDevice(string id)
        {
            var device = await _context.SoilMoisturesFlat.FindAsync(id);
            if (device == null)
            {
                return NotFound("Device not found.");
            }

            _context.SoilMoisturesFlat.Remove(device);
            await _context.SaveChangesAsync();

            return NoContent(); // 204 No Content
        }

        private bool DeviceExists(string id)
        {
            return _context.SoilMoisturesFlat.Any(e => e.DeviceId == id);
        }
    }
}
