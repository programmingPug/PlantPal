# Use the official .NET SDK image for building the application
FROM mcr.microsoft.com/dotnet/sdk:9.0 AS build
WORKDIR /src

# Copy the csproj file and restore dependencies
COPY ["SoilMoistureAPI.csproj", "./"]
RUN dotnet restore "SoilMoistureAPI.csproj"

# Copy the remaining source code and build the project
COPY . .
RUN dotnet build "SoilMoistureAPI.csproj" -c Release -o /app/build

# Publish the application to the /app/publish directory
FROM build AS publish
RUN dotnet publish "SoilMoistureAPI.csproj" -c Release -o /app/publish

# Use the official ASP.NET Core runtime image for running the application
FROM mcr.microsoft.com/dotnet/aspnet:9.0 AS final
WORKDIR /app
COPY --from=publish /app/publish .

#RUN groupadd --gid "1000" "plantpal" && useradd --uid "1000" --gid "1000" -m "plantpal"
#RUN chown plantpal:1000 /app
#USER plantpal

# Expose port 80 (HTTP) and 443 (HTTPS) if needed
EXPOSE 80
#EXPOSE 443

# Set environment variables
ENV ASPNETCORE_URLS=http://+:80

# Set the entry point to run the application
ENTRYPOINT ["dotnet", "SoilMoistureAPI.dll"]
