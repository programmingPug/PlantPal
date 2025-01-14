using Microsoft.EntityFrameworkCore;
using SoilMoistureAPI.Models;

namespace SoilMoistureAPI.Data
{
    public class SoilMoistureContext : DbContext
    {
        public SoilMoistureContext(DbContextOptions<SoilMoistureContext> options) : base(options)
        {
        }

        public DbSet<SoilMoisture> SoilMoistures { get; set; }
        public DbSet<Device> Devices { get; set; } // New DbSet for Devices

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            // Configure the primary key for Device
            modelBuilder.Entity<Device>()
                .HasKey(d => d.DeviceId);

            // Configure one-to-many relationship
            modelBuilder.Entity<Device>()
                .HasMany(d => d.SoilMoistures)
                .WithOne(s => s.Device)
                .HasForeignKey(s => s.DeviceId)
                .OnDelete(DeleteBehavior.Cascade);
        }
    }
}
