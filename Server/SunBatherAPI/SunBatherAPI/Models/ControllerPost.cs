using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace SunBatherAPI.Models
{
    /// <summary>
    /// This class is sent by the Arduino to the database.
    /// </summary>
    public class ControllerPost
    {
        /// <summary>
        /// Contains information about a single sensor record event.
        /// System information is not included here.
        /// </summary>
        public class Record
        {
            /// <summary>
            /// Record index as described by the controller
            /// </summary>
            public int Id;

            /// <summary>
            /// Date and time when the record was created
            /// </summary>
            public DateTime Date;

            /// <summary>
            /// Temperature at inlet.
            /// </summary>
            public float Tin;

            /// <summary>
            /// Temperature at outlet.
            /// </summary>
            public float Tout;

            /// <summary>
            /// Temperature of roof.
            /// </summary>
            public float Troof;
        }

        /// <summary>
        /// Contains system status data.
        /// </summary>
        public class Settings
        {
            /// <summary>
            /// System automatic mode setting.
            /// True for automatic mode, or false for manual mode.
            /// </summary>
            public bool? Auto { get; set; }
        }

        /// <summary>
        /// A unique identifier used for the system.
        /// The controller will know this and must forward it with every message.
        /// </summary>
        public Guid Guid { get; set; }

        /// <summary>
        /// An optional list of new records, which the controller is sending to the database.
        /// </summary>
        public Record[] Records { get; set; }

        /// <summary>
        /// An optional list of new system status data, which the controller is sending to the database.
        /// </summary>
        public Settings Status { get; set; }
    }
}
