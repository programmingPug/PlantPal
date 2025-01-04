namespace house_plant_api.Controllers
{
    using house_plant_api.Context;
    using Microsoft.AspNetCore.Mvc;
    using Microsoft.EntityFrameworkCore;
    using System.Linq;
    using System.Threading.Tasks;

    [ApiController]
    [Route("api/[controller]")]
    public class DevicesController : ControllerBase
    {
        private readonly IDbContextFactory<AppDbContext> _dbContextFactory;

        public DevicesController(IDbContextFactory<AppDbContext> dbContextFactory)
        {
            _dbContextFactory = dbContextFactory;
        }

        // GET: api/devices
        [HttpGet]
        public async Task<IActionResult> GetDevices()
        {
            using var dbContext = _dbContextFactory.CreateDbContext();
            var devices = await dbContext.Devices.ToListAsync();
            var result = devices.Select(d => new
            {
                d.Name,
                d.Uuid,
                d.Nickname,
                Moisture = d.Moisture.HasValue ? $"{d.Moisture}%" : "N/A",
                d.LastSeen
            });
            return Ok(result);
        }

        // GET: api/devices/{name}
        [HttpGet("{name}")]
        public async Task<IActionResult> GetDevice(string name)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();
            var device = await dbContext.Devices.FirstOrDefaultAsync(d => d.Name == name);

            if (device == null)
                return NotFound(new { Error = $"Device '{name}' not found." });

            return Ok(new
            {
                device.Name,
                device.Uuid,
                device.Nickname,
                Moisture = device.Moisture.HasValue ? $"{device.Moisture}%" : "N/A",
                device.LastSeen
            });
        }

        // DELETE: api/devices/{name}
        [HttpDelete("{name}")]
        public async Task<IActionResult> DeleteDevice(string name)
        {
            using var dbContext = _dbContextFactory.CreateDbContext();
            var device = await dbContext.Devices.FirstOrDefaultAsync(d => d.Name == name);

            if (device == null)
                return NotFound(new { Error = $"Device '{name}' not found in database." });

            dbContext.Devices.Remove(device);
            await dbContext.SaveChangesAsync();

            return Ok(new { Message = $"Device '{name}' removed successfully." });
        }

        // PUT: api/devices/{name}/nickname
        [HttpPut("{name}/nickname")]
        public async Task<IActionResult> UpdateNickname(string name, [FromBody] NicknameUpdateRequest request)
        {
            // Find the device by Name
            using var dbContext = _dbContextFactory.CreateDbContext();
            var device = await dbContext.Devices.FirstOrDefaultAsync(d => d.Name == name);

            if (device == null)
            {
                return NotFound(new { Error = $"Device '{name}' not found." });
            }

            // Update the Nickname
            device.Nickname = request.Nickname;

            // Save changes
            dbContext.Devices.Update(device);
            await dbContext.SaveChangesAsync();

            return Ok(new
            {
                Message = $"Nickname updated successfully for device '{name}'.",
                Nickname = device.Nickname
            });
        }

    }

    // A simple DTO (Data Transfer Object) for updating the nickname
    public class NicknameUpdateRequest
    {
        public string Nickname { get; set; }
    }

}
