using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace SunBatherAPI.Models
{
    /// <summary>
    /// This class is sent from the database to the controller.
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
    }
}
