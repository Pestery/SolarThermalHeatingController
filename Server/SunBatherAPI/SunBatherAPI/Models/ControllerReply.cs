using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace SunBatherAPI.Models
{
    /// <summary>
    /// This class is sent from the database, to the Controller/Arduino.
    /// </summary>
    public class ControllerReply
    {
        /// <summary>
        /// System automatic mode setting.
        /// True for automatic mode, or false for manual mode.
        /// </summary>
        public bool? Auto { get; set; }

        /// <summary>
        /// The temperature the controller will try to maintain when it automatic mode.
        /// </summary>
        public float? SetTemp { get; set; }

        /// <summary>
        /// Manual mode pump-on status.
        /// If the controller is set to manual mode then if this value will set the pump to always on or off.
        /// </summary>
        public bool? PumpOn { get; set; }

        /// <summary>
        /// The DateTime of the last received record.
        /// </summary>
        public long? LastRecord { get; set; }
    }
}
