namespace house_plant_api.Context
{
    using house_plant_api.Models;
    using Microsoft.EntityFrameworkCore;

    public class AppDbContext : DbContext
    {
        public DbSet<Device> Devices { get; set; }

        public AppDbContext(DbContextOptions<AppDbContext> options) : base(options) { }

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            modelBuilder.Entity<Device>()
                .HasKey(d => d.Id);

            modelBuilder.Entity<Device>()
                .Property(d => d.Name)
                .IsRequired();

            modelBuilder.Entity<Device>()
                .Property(d => d.Nickname);

            modelBuilder.Entity<Device>()
                .Property(d => d.Uuid)
                .IsRequired();
        }
    }
}
