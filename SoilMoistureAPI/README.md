
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

## API features

### Flattened Data Model:

- The API uses a single, flat model (e.g., SoilMoistureFlat) that contains all necessary information, thereby avoiding nested or circular references.
- The model includes fields such as:
  - deviceId: A unique identifier for the sensor (such as its BLE address).
  - moistureLevel: The current moisture reading (as a float).
  - timestamp: The time the record was created or updated (set to UTC on the server).
  - deviceNickname: A friendly name for the device (e.g., "SoilSensor").

### Combined Insert/Update Logic (Upsert):

- The PostSoilMoisture method accepts a flattened JSON payload.
- If a record for a given deviceId already exists, the API updates its moistureLevel and timestamp (and updates the deviceNickname if provided).
- If no record exists for that deviceId, a new record is inserted with the given information.

### Validation:

- The API validates incoming data and ensures required fields are present (such as deviceId and moistureLevel).
- It returns detailed error messages if validation fails, helping clients correct their payloads.

### Simplified Data Handling:

- With a flattened model, there is no need for complex navigation properties or sub-object mapping; all relevant data is stored in one entity.
- This design simplifies both the client payload construction and server-side processing.

### Timestamp Management:

- The API sets (or updates) the timestamp (using the current UTC time) whenever a record is created or modified.
- This provides a reliable audit trail for when sensor data was recorded or updated.