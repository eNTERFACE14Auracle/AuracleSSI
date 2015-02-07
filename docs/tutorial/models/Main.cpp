// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2008/04/24
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
#include "MyModel.h"
#include "MyFusion.h"
using namespace ssi;

#define N_COLORS 5
unsigned short COLORS[][3] = {
	128,0,0,
	0,128,0,
	0,0,128,
	128,0,128,
	0,128,128
};

void ex_model ();
void ex_fusion ();
void ex_online ();
void PlotSamples (ISamples &samples, const ssi_char_t *name);

int main () {

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssiframe.dll");
	Factory::RegisterDLL ("ssievent.dll");
	Factory::RegisterDLL ("ssimodel.dll");
	Factory::RegisterDLL ("ssimouse.dll");
	Factory::RegisterDLL ("ssigraphic.dll");

	Factory::Register (MyModel::GetCreateName (), MyModel::Create);
	Factory::Register (MyFusion::GetCreateName (), MyFusion::Create);

	ssi_random_seed ();

	ex_model ();

	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	ex_fusion ();

	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	ex_online ();

	ssi_print ("\n\n\tpress enter to quit\n");
	getchar ();

	Factory::Clear ();

	return 0;
}

void ex_model () {

	ssi_size_t n_classes = 4;
	ssi_size_t n_sampels = 50;
	ssi_size_t n_streams = 1;

	SampleList strain, sdevel;
	{
		ssi_real_t distr[][3] = { 0.33f, 0.33f, 0.15f, 0.33f, 0.66f, 0.15f, 0.66f, 0.33f, 0.15f, 0.66f, 0.66f, 0.15f};
		ModelTools::CreateTestSamples (strain, n_classes, n_sampels, n_streams, distr);	
		ModelTools::CreateTestSamples (sdevel, n_classes, n_sampels, n_streams, distr);
	}

	SampleList stest;
	{
		ssi_real_t distr[][3] = { 0.5f, 0.5f, 0.5f };
		ModelTools::CreateTestSamples (stest, 1, n_sampels * n_classes, n_streams, distr);	
		ssi_char_t string[SSI_MAX_CHAR];	
		for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
			ssi_sprint (string, "class%02d", n_class);
			stest.addClassName (string);
		}
	}

	{
		MyModel *model = ssi_pcast (MyModel, Factory::Create (MyModel::GetCreateName ()));

		Trainer trainer (model, 0);
		trainer.train (strain);
		trainer.save ("mymodel");
	}

	{
		Trainer trainer;
		Trainer::Load (trainer, "mymodel");

		Evaluation eval;
		eval.eval (trainer, sdevel);
		eval.print ();
		
		PlotSamples (strain, "training samples");
		PlotSamples (stest, "test samples");
		trainer.cluster (stest);
		PlotSamples (stest, "clustered test samples");
	}
}

void ex_fusion () {

	ssi_size_t n_classes = 4;
	ssi_size_t n_sampels = 50;
	ssi_size_t n_streams = 5;

	SampleList strain, sdevel;
	{
		ssi_real_t distr[][3] = { 0.33f, 0.33f, 0.15f, 0.33f, 0.66f, 0.15f, 0.66f, 0.33f, 0.15f, 0.66f, 0.66f, 0.15f};
		ModelTools::CreateTestSamples (strain, n_classes, n_sampels, n_streams, distr);	
		ModelTools::CreateTestSamples (sdevel, n_classes, n_sampels, n_streams, distr);
	}

	{
		IModel **models = new IModel *[n_streams];
		for (ssi_size_t i = 0; i < n_streams; i++) {
			models[i] = ssi_pcast (MyModel, Factory::Create (MyModel::GetCreateName ()));
		}
		MyFusion *fusion = ssi_pcast (MyFusion, Factory::Create (MyFusion::GetCreateName ()));

		Trainer trainer (n_streams, models, fusion);
		trainer.train (strain);
		trainer.save ("myfusion");
	}

	{
		Trainer trainer;
		Trainer::Load (trainer, "myfusion");

		Evaluation eval;
		eval.eval (trainer, sdevel);
		eval.print ();
	}
}

void ex_online () {

	ITheFramework *frame = Factory::GetFramework ();

	ssi_size_t n_classes = 4;
	ssi_size_t n_sampels = 50;
	ssi_size_t n_streams = 1;
	ssi_real_t distr[][3] = { 0.66f, 0.33f, 0.15f, 0.33f, 0.33f, 0.15f, 0.66f, 0.66f, 0.15f, 0.33f, 0.66f, 0.15f };
	SampleList strain;
	ModelTools::CreateTestSamples (strain, n_classes, n_sampels, n_streams, distr);	

	MyModel *model = ssi_pcast (MyModel, Factory::Create (MyModel::GetCreateName ()));
	Trainer trainer (model, 0);
	trainer.train (strain);
	
	Mouse *mouse = ssi_pcast (Mouse, Factory::Create (Mouse::GetCreateName (), "mouse"));
	ITransformable *cursor_p = frame->AddProvider (mouse, SSI_MOUSE_CURSOR_PROVIDER_NAME);
	frame->AddSensor (mouse); 
				
	Classifier *classifier = ssi_pcast (Classifier, Factory::Create (Classifier::GetCreateName ()));
	classifier->setTrainer (trainer);
	frame->AddConsumer (cursor_p, classifier, "0.5s");

	frame->Start ();
	getchar ();
	frame->Stop ();
	frame->Clear ();
}

void PlotSamples (ISamples &samples, const ssi_char_t *name) {

	ThePainter *painter = (ThePainter *) Factory::Create(ThePainter::GetCreateName(), 0, false);

	int plot_id = painter->AddCanvas (name);

	ssi_size_t n_classes = samples.getClassSize ();	
	PaintData<ssi_real_t> **plots = new PaintData<ssi_real_t> *[n_classes];
	
	const ssi_real_t min[2] = {0.0f, 0.0f};
	const ssi_real_t max[2] = {1.0f, 1.0f};
	for (ssi_size_t n_class = 0; n_class < n_classes; n_class++) {

		plots[n_class] = new PaintData<ssi_real_t> (2, PAINT_TYPE_SCATTER);

		ssi_size_t n_samples = samples.getSize (n_class);
		ssi_size_t n_streams = samples.getStreamSize ();
		ssi_real_t *data = new ssi_real_t[n_samples * 2 * n_streams];
		samples.reset ();
		ssi_sample_t *sample = 0;
		ssi_real_t *data_ptr = data;
		ISSelectClass selclass (&samples);		
		selclass.setSelection (n_class);
		selclass.reset ();
		while (sample = selclass.next ()) {
			for (ssi_size_t n_stream = 0; n_stream < n_streams; n_stream++) {
				memcpy (data_ptr, sample->streams[n_stream]->ptr, 2 * sizeof (ssi_real_t));
				data_ptr += 2;
			}
		}

		plots[n_class]->setPointSize (10);
		plots[n_class]->setFixedMinMax (min, max);
		plots[n_class]->setData (data, n_samples * n_streams);		
		plots[n_class]->setFillColor (COLORS[n_class][0],COLORS[n_class][1],COLORS[n_class][2]);
		painter->AddObject (plot_id, plots[n_class]);

		delete[] data;
	}

	painter->Paint (plot_id); 
	painter->Move (plot_id, 0, 0, 600, 600);
	painter->MoveConsole (0, 600, 600, 400);

	ssi_print ("\n\tpress enter to close plot window!\n\n");
	getchar ();

	for (ssi_size_t n_class = 0; n_class < n_classes; n_class++) {
		delete plots[n_class];
	}
	delete[] plots;

	painter->Clear ();
}
