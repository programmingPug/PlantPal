using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace SoilMoistureAPI.Models
{
    public class Device
    {
        [Key]
        public string DeviceId { get; set; } // Primary Key

        [Required]
        [MaxLength(100)]
        public string Nickname { get; set; }

        // Navigation Property
        public ICollection<SoilMoisture> SoilMoistures { get; set; }
    }
}
