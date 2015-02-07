// ssi.h
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/09/05
// Copyright (C) 2007-10 University of Augsburg, Johannes Wagner
//
// *************************************************************************************************
//
// This file is part of Smart Sensor Integration (SSI) developed at the 
// Lab for Human Centered Multimedia of the University of Augsburg
//
// This library is free software; you can redistribute itand/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or any laterversion.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FORA PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along withthis library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//
//*************************************************************************************************

#pragma once

#ifndef SSI_H
#define	SSI_H

#include "ssiaudio.h"
#include "ssicamera.h"
#include "ssiioput.h"
#include "ssimouse.h"
#include "ssimodel.h"
#include "ssisignal.h"
#include "ssigraphics.h"
#include "ssievent.h"
#include "ssiframe.h"
#include "ssiimage.h"

#include "base/Factory.h"
#include "base/Array1D.h"

#include "thread/Thread.h"
#include "thread/Event.h"
#include "thread/Lock.h"
#include "thread/Mutex.h"
#include "thread/Timer.h"
#include "thread/Condition.h"
#include "thread/ThreadPool.h"
#include "thread/RunAsThread.h"

#include "ioput/option/OptionList.h"
#include "ioput/option/CmdArgOption.h"
#include "ioput/option/CmdArgParser.h"
#include "ioput/file/File.h"
#include "ioput/file/FileAscii.h"
#include "ioput/file/FileMem.h"
#include "ioput/file/FileMemAscii.h"
#include "ioput/file/FileBinary.h"
#include "ioput/file/FileTools.h"
#include "ioput/file/FilePath.h"
#include "ioput/file/StringList.h"
#include "ioput/file/FileAnnotationWriter.h"
#include "ioput/file/FileProvider.h"
#include "ioput/file/FileStreamIn.h"
#include "ioput/file/FileStreamOut.h"
#include "ioput/file/FileSamplesIn.h"
#include "ioput/file/FileSamplesOut.h"
#include "ioput/file/FileEventsIn.h"
#include "ioput/file/FileEventsOut.h"
#include "ioput/file/FileMessage.h"
#include "ioput/socket/Socket.h"
#include "ioput/socket/SocketUdp.h"
#include "ioput/socket/SocketTcp.h"
#include "ioput/socket/SocketOsc.h"
#include "ioput/socket/SocketOscListener.h"
#include "ioput/socket/SocketOscEventWriter.h"
#include "ioput/socket/SocketImage.h"
#include "ioput/socket/SocketMessage.h"
#include "ioput/pipe/NamedPipe.h"
#include "ioput/arff/SampleArff.h"
#include "ioput/elan/ElanDocument.h"
#include "ioput/window/Slider.h"
#include "ioput/window/Monitor.h"

#include "graphic/PaintData.h"

#include "model/ModelTools.h"
#include "model/SampleList.h"
#include "model/Trainer.h"
#include "model/Annotation.h"
#include "model/Evaluation.h"
#include "model/Evaluation2Latex.h"
#include "model/Selection.h"
#include "model/ISHotClass.h"
#include "model/ISReClass.h"
#include "model/ISTransform.h"
#include "model/ISSelectDim.h"
#include "model/ISSelectSample.h"
#include "model/ISSelectClass.h"
#include "model/ISSelectUser.h"
#include "model/ISMergeStrms.h"
#include "model/ISAlignStrms.h"
#include "model/ISMissingData.h"
#include "model/ISSplitStream.h"
#include "model/ISMergeSample.h"
#include "model/ISOverSample.h"
#include "model/ISUnderSample.h"
#include "model/ISNorm.h"
#include "model/ISDuplStrms.h"
#include "model/ISTrigger.h"

#include "signal/Matrix.h"
#include "signal/MatrixOps.h"
#include "signal/SignalCons.h"
#include "signal/SignalTools.h"
#include "signal/FilterTools.h"
#include "signal/FFT.h"
#include "signal/IFFT.h"
#include "signal/Cast.h"
#include "signal/FFTfeat.h"
#include "signal/Operator0.h"
#include "signal/Operator1.h"

#include "event/EventAddress.h"

#endif
