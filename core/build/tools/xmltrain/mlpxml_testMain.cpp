// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/11/23 
// Copyright (C) 2007-13 University of Augsburg, Lab for Human Centered Multimedia
//
// *************************************************************************************************
//
// This file is part of Social Signal Interpretation (SSI) developed at the 
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

#include "ssi.h"
#include "ssimlpxml.h"
#include "MlpXmlTrain.h"
using namespace ssi;

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

void run (IMlpXml *mlp, bool record);

class MyCallback : public MlpXmlICallback {
public:		
	void call (double time, double dur, const char *label, bool store, bool change) {
		ssi_print ("%.2lfs@%.2lfs : %s\n", dur, time, label);
	};
};

int main () {

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssiioput.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssisignal.dll");
	Factory::RegisterDLL ("ssimlpxml.dll");
	Factory::RegisterDLL ("ssimodel.dll");

	{
		// create
		IMlpXml *mlp = ssi_pcast (IMlpXml, ssi_create (MlpXml, "mlp", true));	
		MyCallback callback;
		mlp->setCallback (&callback);
		MlpXmlTrain train ("mlp\\mlp");

		// record
		mlp->getOptions ()->setOptionValue ("trainer", "");
		mlp->getOptions ()->setOptionValue ("anno", "button");
		mlp->getOptions ()->setOptionValue ("signal", "cursor");
		run (mlp, true);

		// train	
		ssi_size_t n_defs = 0;
		ssi_char_t *const*defs = train.getDefNames (n_defs);
		for (ssi_size_t i = 0; i < n_defs; i++) {
			ssi_print ("%s\n", defs[i]);
		}
		train.init ("dollar", "cursor", MlpXmlDef::STREAM, "button");
		train.collect ("mlp\\data\\user\\2011-04-27_18-54-47", true);
		train.collect ("mlp\\data\\user\\2011-05-30_17-13-31", true);
		train.save ("mlp\\mlp");
		train.train ("mlp\\mlp");

		// classify
		mlp->getOptions ()->setOptionValue ("trainer", "mlp\\mlp");
		mlp->getOptions ()->setOptionValue ("anno", "");
		mlp->getOptions ()->setOptionValue ("signal", "");
		run (mlp, false);
	}

	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void run (IMlpXml *mlp, bool record) {

	ITheFramework *frame = Factory::GetFramework ();
	IThePainter *painter = Factory::GetPainter ();
		
	Mouse *mouse = ssi_pcast (Mouse, Factory::Create (Mouse::GetCreateName (), "mouse"));
	mouse->getOptions ()->mask = Mouse::LEFT;
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	ITransformable *button_p = frame->AddProvider (mouse, SSI_MOUSE_BUTTON_PROVIDER_NAME);
	frame->AddSensor (mouse);

	ITransformable *its[] = {cursor_p, button_p};
	frame->AddConsumer (2, its, mlp, "0.2s");

	frame->Start ();
	if (record) {
		ssi_print ("draw rectangles for 10 seconds (to draw a gesture press left mouse button, draw and release)..\n");
		mlp->setLabel ("rectangle");
		Sleep (10000);
		ssi_print ("draw triangles for 10 seconds..\n");
		mlp->setLabel ("triangle");
		Sleep (10000);
		ssi_print ("draw circles for 10 seconds..\n");
		mlp->setLabel ("circle");
		Sleep (10000);
	} else {
		ssi_print ("\n\tpress enter to stop\n\n");
		getchar ();
	}

	frame->Stop ();
	frame->Clear ();
	painter->Clear ();
}
