using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace SunBatherAPI.Models
{
    /// <summary>
    /// This class is sent by the Controller/Arduino, to the database.
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
            public int Id { get; set; }

            /// <summary>
            /// Date and time when the record was created
            /// </summary>
            public long Date { get; set; }

            /// <summary>
            /// Temperature at inlet.
            /// </summary>
            public float Tin { get; set; }

            /// <summary>
            /// Temperature at outlet.
            /// </summary>
            public float Tout { get; set; }

            /// <summary>
            /// Temperature of roof.
            /// </summary>
            public float Troof { get; set; }
        }

        /// <summary>
        /// Contains system status data.
        /// </summary>
        public class Settings
        {
            /// <summary>
            /// Current pump status.
            /// True if the pump is currently running, or false if not.
            /// </summary>
            public bool? PumpOn { get; set; }
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
