using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using SunBatherAPI.Data;

namespace SunBatherAPI.Models
{
    public class SystemIdentity
    {
        public int Id { get; set; }
        public Guid ProductId { get; set; }
    }
}
