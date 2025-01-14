using Microsoft.EntityFrameworkCore;
using SoilMoistureAPI.Data;
//using SoilMoistureAPI.Middleware;

var builder = WebApplication.CreateBuilder(args);

builder.WebHost.ConfigureKestrel(options =>
{
    options.ListenAnyIP(80); // Listen on port 80 for HTTP
    // Optionally, listen on port 443 for HTTPS
    /*
    options.ListenAnyIP(443, listenOptions =>
    {
        listenOptions.UseHttps();
    });
    */
});

// Add services to the container.
builder.Services.AddControllers();

// Configure CORS
builder.Services.AddCors(options =>
{
    options.AddPolicy("AllowClient",
        builder =>
        {
            //builder.WithOrigins("http://localhost:3000", "http://soilmoisture_client:80")
            builder.AllowAnyOrigin()
                   .AllowAnyHeader()
                   .AllowAnyMethod();
        });
});

// Register the DB context with SQLite
builder.Services.AddDbContext<SoilMoistureFlatContext>(options =>
    options.UseSqlite(builder.Configuration.GetConnectionString("DefaultConnection")));

// Add Swagger/OpenAPI support
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

var app = builder.Build();

// Apply migrations at startup
using (var scope = app.Services.CreateScope())
{
    var db = scope.ServiceProvider.GetRequiredService<SoilMoistureFlatContext>();
    db.Database.Migrate();
}

// Use API Key Middleware
//app.UseMiddleware<ApiKeyMiddleware>();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseHttpsRedirection();

app.UseCors("AllowClient");

app.UseAuthorization();

app.MapControllers();

app.Run();
