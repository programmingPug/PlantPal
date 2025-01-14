using System;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace SoilMoistureAPI.Models
{
    public class SoilMoisture
    {
        [Key]
        public int Id { get; set; }

        [Required]
        [ForeignKey("Device")]
        public string DeviceId { get; set; } // Foreign Key

        [Required]
        public float MoistureLevel { get; set; }

        public DateTime Timestamp { get; set; } = DateTime.UtcNow;

        // Navigation Property
        public Device Device { get; set; }
    }
}
