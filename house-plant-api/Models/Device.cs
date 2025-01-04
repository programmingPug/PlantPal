namespace house_plant_api.Models
{
    using System;

    public class Device
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string Nickname { get; set; }
        public string Uuid { get; set; }
        public int? Moisture { get; set; }
        public DateTime? LastSeen { get; set; }
    }
}
