using Microsoft.AspNetCore.Builder;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.EntityFrameworkCore;
using house_plant_api.Context;
using house_plant_api.Services;

var builder = WebApplication.CreateBuilder(args);

// 1. Register EF Core with DbContextFactory
builder.Services.AddDbContextFactory<AppDbContext>(options =>
    options.UseSqlite("Data Source=devices.db"));

// 2. Register the BluetoothService (can be Singleton or Transient)
builder.Services.AddSingleton<BluetoothService>();

// 3. Register the hosted service
builder.Services.AddHostedService<DevicePollingService>();

// 4. Add controllers
builder.Services.AddControllers();

// 1. Register CORS services
builder.Services.AddCors(options =>
{
    options.AddPolicy("AllowAll", policyBuilder =>
    {
        policyBuilder
            .AllowAnyOrigin()  // Allows requests from any domain
            .AllowAnyHeader()  // Allows any request headers
            .AllowAnyMethod(); // Allows any HTTP method (GET, POST, PUT, etc.)
    });
});


var app = builder.Build();

// 3. Use CORS with the named policy
app.UseCors("AllowAll");

// Ensure DB is created or migrated
using (var scope = app.Services.CreateScope())
{
    var dbContextFactory = scope.ServiceProvider.GetRequiredService<IDbContextFactory<AppDbContext>>();
    using var dbContext = dbContextFactory.CreateDbContext();
    dbContext.Database.EnsureCreated(); // or dbContext.Database.Migrate() if using migrations
}

app.MapControllers();

app.Run();