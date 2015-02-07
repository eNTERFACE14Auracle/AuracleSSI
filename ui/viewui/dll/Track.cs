using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using AC.AvalonControlsLibrary.Controls;
using System.Windows.Input;

namespace ssi
{
    public interface ITrack
    {
        void timeRangeChanged(ViewTime time);
    }
}
