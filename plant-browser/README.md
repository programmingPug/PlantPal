
<br/>
<div align="center">
<a href="https://github.com/programmingPug/plantpal">
<img src="https://imgur.com/BdczfrQ.png" alt="Logo" width="80" height="80">
</a>
<h3 align="center">PlantPal</h3>
<p align="center">
Because knowing when to water your plants is hard
</p>
</div>

## Client features

### Modern Standalone Components:
- The client is built (e.g., in Angular) using standalone components to simplify development, improve reusability, and ensure easier integration with the REST API.

### Dashboard Overview:
- The UI provides a comprehensive dashboard displaying key sensor data such as soil moisture levels, battery percentages, and timestamps for the latest readings.

### Real-Time & Historical Data:

- Real-Time Updates: The client periodically (or via WebSockets, if implemented) fetches the latest sensor data from the API for immediate display.
- Historical Visualization: Graphs and charts can illustrate trends over time (if historical data is supported by the backend).

### Device Management:

- Editing: Users can click on a device’s name to open a dialog (styled consistently with the parent application) and update its nickname.
- Removal: The interface provides an endpoint for removing devices from the monitoring system.
- Configuration: The client is set up to use environment-specific settings (e.g., pointing to a production API endpoint over CORS-friendly URLs).

### REST API Integration:
- The application communicates with the backend API seamlessly, handling CORS policies appropriately. It uses Angular’s HttpClient (or similar libraries) to manage GET/POST requests to retrieve and update soil moisture data.

### Swagger and Documentation:
- The client (and possibly the developer portal) references Swagger documentation available for the API, ensuring that API changes are clearly documented and easily testable.

### Responsive Design:
- The application is responsive and designed for both mobile and desktop use—making it easy for users in the field or in the office to quickly assess sensor status and history.

### Customizable & Environment-Aware:
- Configuration for connecting to the API (base URL, API key) is managed via environment variables or configuration files so that the client can be adapted between development, staging, and production without code changes.