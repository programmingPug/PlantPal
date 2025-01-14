using Microsoft.EntityFrameworkCore;
using SoilMoistureAPI.Models;

namespace SoilMoistureAPI.Data
{
    public class SoilMoistureFlatContext : DbContext
    {
        public DbSet<SoilMoistureFlat> SoilMoisturesFlat { get; set; }

        public SoilMoistureFlatContext(DbContextOptions<SoilMoistureFlatContext> options)
            : base(options)
        {
        }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            // Optionally configure the SoilMoistureFlat entity
            modelBuilder.Entity<SoilMoistureFlat>(entity =>
            {
                entity.HasKey(e => e.Id);
                entity.Property(e => e.DeviceId)
                      .IsRequired();
                entity.Property(e => e.MoistureLevel)
                      .IsRequired();
                entity.Property(e => e.Nickname)
                      .IsRequired();

                // Optionally, set default SQL for Timestamp if using SQL Server
                // entity.Property(e => e.Timestamp).HasDefaultValueSql("GETUTCDATE()");
            });

            base.OnModelCreating(modelBuilder);
        }
    }
}
