using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ssi
{
    public interface ISignalTrack : ITrack
    {
        Signal getSignal();
    }
}
