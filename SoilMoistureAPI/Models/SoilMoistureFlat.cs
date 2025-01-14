namespace SoilMoistureAPI.Models
{
    public class SoilMoistureFlat
    {
        public int Id { get; set; }
        public string DeviceId { get; set; }         // Unique identifier for the sensor device
        public float MoistureLevel { get; set; }       // Current moisture reading
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;
        public string Nickname { get; set; }     // A friendly name for the device
    }

}
