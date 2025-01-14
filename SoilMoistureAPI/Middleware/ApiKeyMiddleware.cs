/*
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Configuration;
using System.Linq; // Ensure this namespace is included
using System.Threading.Tasks;

namespace SoilMoistureAPI.Middleware
{
    public class ApiKeyMiddleware
    {
        private readonly RequestDelegate _next;
        private const string APIKEYNAME = "X-API-KEY";
        private readonly string[] _validApiKeys;

        public ApiKeyMiddleware(RequestDelegate next, IConfiguration configuration)
        {
            _next = next;
            _validApiKeys = configuration.GetSection("ApiSettings:ValidApiKeys").Get<string[]>();
        }

        public async Task InvokeAsync(HttpContext context)
        {
            if (!context.Request.Headers.TryGetValue(APIKEYNAME, out var extractedApiKey))
            {
                context.Response.StatusCode = 401; // Unauthorized
                await context.Response.WriteAsync("API Key was not provided.");
                return;
            }

            // Convert StringValues to string using FirstOrDefault()
            var apiKey = extractedApiKey.FirstOrDefault();

            if (string.IsNullOrEmpty(apiKey) || !_validApiKeys.Contains(apiKey))
            {
                context.Response.StatusCode = 403; // Forbidden
                await context.Response.WriteAsync("Unauthorized client.");
                return;
            }

            await _next(context);
        }
    }
}
*/