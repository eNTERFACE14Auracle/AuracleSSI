// Main.cpp
// author: Johannes Wagner <wagner@hcm-lab.de>
// created: 2007/10/11
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
using namespace ssi;

#define N_COLORS 5
unsigned short COLORS[][3] = {
	128,0,0,
	0,128,0,
	0,0,128,
	128,0,128,
	0,128,128
};

void ex_random ();
void ex_samplelist ();
void ex_eval ();
void ex_model (); 
void ex_fusion ();	

void PlotSamples (ISamples &samples, const ssi_char_t *name);
void CreateMissingData (SampleList &samples, double prob);

#ifdef USE_SSI_LEAK_DETECTOR
	#include "SSI_LeakWatcher.h"
	#ifdef _DEBUG
		#define new DEBUG_NEW
		#undef THIS_FILE
		static char THIS_FILE[] = __FILE__;
	#endif
#endif

int main () {	

#ifdef USE_SSI_LEAK_DETECTOR
	{
#endif

	ssi_print ("%s\n\nbuild version: %s\n\n", SSI_COPYRIGHT, SSI_VERSION);

	Factory::RegisterDLL ("ssimodel.dll");
	Factory::RegisterDLL ("ssigraphic.dll");
	Factory::RegisterDLL ("ssisignal.dll");

	ssi_random_seed ();

	ex_random ();

	Factory::GetPainter ()->MoveConsole (0,600,600,400);

	ex_samplelist ();
	ex_eval ();
	ex_model (); 
	ex_fusion ();	

	ssi_print ("\n\n\tpress enter to quit\n\n");
	getchar ();

	Factory::Clear ();

#ifdef USE_SSI_LEAK_DETECTOR
	}
	_CrtDumpMemoryLeaks();
#endif
	
	return 0;
}

void ex_random () {

	unsigned int seed = ssi_cast (unsigned int, time (NULL));
	ssi_size_t n = 100000;

	ssi_tic ();
	for (ssi_size_t i = 0; i < n; i++) {
		ssi_random ();
	}
	ssi_toc_print ();

	ssi_print ("\n");

	ssi_tic ();
	for (ssi_size_t i = 0; i < n; i++) {
		ssi_random_distr (0,1);
	}
	ssi_toc_print ();

	ssi_print ("\n");
}

void ex_samplelist () {

	ssi_char_t now[ssi_time_size];

	{
		ssi_now (now);
		ssi_print ("%s\n", now);
		time_t now_t = ssi_time_parse (now);
		ssi_time_format (now_t, now);
		ssi_print ("%s\n", now);

		ssi_char_t *base_dir = "in\\";

		ssi_print ("read files from %s\n", base_dir);

		// load
		StringList files;
		FileTools::ReadFilesFromDir (files, base_dir, "*.feat");
		files.print (stdout);
		SampleList samples;
		ModelTools::LoadSampleList (samples, files);

		ssi_print ("merged..\n");
		samples.print (stdout);
		
		// export to arff
		ModelTools::SaveSampleListArff (samples, "samples.arff");

		// clone
		SampleList samples_clone;
		ModelTools::CopySampleList (samples, samples_clone);
		ModelTools::SaveSampleList (samples, "out", ".feat");

		ssi_print ("cloned..\n");
		samples_clone.print (stdout);

		SampleList samples_merge;
		SampleList *samples_from[2] = {&samples, &samples_clone};
		ModelTools::MergeSampleList (samples_merge, 2, samples_from);

		ssi_print ("merged..\n");
		samples_merge.print (stdout);
	}

	{
		Annotation anno;
		ModelTools::LoadAnnotation (anno, "in/anno.txt");
		Annotation::Entry *e = 0;
		anno.reset ();
		while (e = anno.next ()) {
			printf ("%.2lf - %.2lf = %s\n", e->start, e->stop, anno.getLabel (e->label_index));
		}
		anno.reset ();

		ssi_stream_t *streams[2];
		
		streams[0] = new ssi_stream_t;
		FileTools::ReadStreamFile (File::BINARY, "in/audio.dat", *streams[0]);

		streams[1] = new ssi_stream_t;		
		FileTools::ReadStreamFile (File::BINARY, "in/wii.dat", *streams[1]);

		SampleList samples;
		ModelTools::LoadSampleList (samples, 2, streams, anno, "user");
		samples.print (stdout);
		ModelTools::SaveSampleList (samples, "out", ".dat");
		{
			ModelTools::SaveSampleList (samples, "out/samples.samples", File::BINARY, File::V1);
		}
		SampleList samples_check, samples_check_2;
		StringList files;
		FileTools::ReadFilesFromDir (files, "out", "*.dat");
		ModelTools::LoadSampleList (samples_check, files);	
		ModelTools::LoadSampleList (samples_check_2, "out/samples.samples", File::BINARY);
		samples_check.print (stdout);
		samples_check_2.print (stdout);

		ssi_stream_destroy (*streams[0]);
		delete streams[0];
		ssi_stream_destroy (*streams[1]);
		delete streams[1];
	}

	{
		Functionals *functs = ssi_pcast (Functionals, Factory::Create (Functionals::GetCreateName ()));

		SampleList samples;
		ModelTools::LoadSampleList (samples, "out/samples.samples", File::BINARY);

		ssi_print ("samples:\n");
		ModelTools::PrintInfo (samples);

		ssi_print ("samples (trans):\n");
		ISTransform samples_trans (&samples);
		samples_trans.setTransformer (1, *functs);
		samples_trans.callEnter ();
		ModelTools::PrintInfo (samples_trans);

		ssi_print ("samples (select dim):\n");
		ISSelectDim samples_seldim (&samples_trans);
		ssi_size_t dims[] = {0,2,4};
		samples_seldim.setSelection (1, 3, dims);
		ModelTools::PrintInfo (samples_seldim);

		ssi_print ("samples (select class):\n");
		ISSelectClass samples_selclass (&samples_seldim);
		ssi_size_t classes[] = {0,2};
		samples_selclass.setSelection (2, classes);
		ModelTools::PrintInfo (samples_selclass);

		ssi_print ("samples (hot):\n");
		ISHotClass samples_hot (&samples_seldim);
		samples_hot.setHotClass (1);
		ModelTools::PrintInfo (samples_hot);

		ModelTools::PrintClasses (samples_seldim);
		ModelTools::PrintClasses (samples_hot);

		ssi_print ("samples (2xhot):\n");
		ISHotClass samples_hot2 (&samples_seldim);
		ssi_size_t hotties[2] = {0,2};
		samples_hot2.setHotClass (2, hotties, "labelAC");
		ModelTools::PrintInfo (samples_hot2);

		ModelTools::PrintClasses (samples_seldim);
		ModelTools::PrintClasses (samples_hot2);

		ssi_print ("samples (re-class):\n");
		ISReClass samples_re (&samples_trans);
		ssi_size_t n_classes = 4;
		const ssi_char_t *class_names[] = {"reA", "reB", "reC", "reD"};
		ssi_size_t n_ids = samples_trans.getSize ();
		ssi_size_t *ids = new ssi_size_t[n_ids];
		for (ssi_size_t i = 0; i < n_ids; i++) {
			ids[i] = i % n_classes;
		}
		samples_re.setReClass (n_classes, class_names, n_ids, ids);
		ModelTools::PrintInfo (samples_re);
		delete[] ids;

		ModelTools::PrintClasses (samples_trans);
		ModelTools::PrintClasses (samples_re);

		ssi_print ("samples (select samps):\n");
		ssi_size_t samps[2] = { 0, 2 };		
		ISSelectSample sample_selsamp (&samples_hot);
		sample_selsamp.setSelection (2, samps);
		ModelTools::PrintInfo (sample_selsamp);

		samples_trans.callFlush ();
	}

	{
		SampleList samples;
		ModelTools::LoadSampleList (samples, "out/samples.samples", File::BINARY);

		ssi_print ("samples:\n");
		ModelTools::PrintInfo (samples);

		FileSamplesOut samples_out;
		samples_out.open (samples, "test", File::BINARY);
		ssi_sample_t *sample = 0;
		samples.reset ();
		while (sample = samples.next ()) {
			samples_out.write (*sample);
		}
		samples_out.write (samples);
		samples_out.close ();
	}

	{
		FileSamplesIn samples_in;
		samples_in.open ("test");
		ModelTools::PrintInfo (samples_in);
		ModelTools::PrintSample (samples_in, 2);
		samples_in.close ();
	}

	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();
}

void ex_eval () {

	ssi_size_t n_classes = 2;
	ssi_size_t n_samples = 20;
	ssi_size_t n_streams = 1;
	ssi_real_t train_distr[][3] = { 0.3f, 0.3f, 0.2f, 0.3f, 0.6f, 0.2f, 0.6f, 0.3f, 0.2f, 0.6f, 0.6f, 0.2f };
	ssi_real_t test_distr[][3] = { 0.5f, 0.5f, 0.5f };
	SampleList samples;		
	ModelTools::CreateTestSamples (samples, n_classes, n_samples, n_streams, train_distr);	
	ssi_char_t string[SSI_MAX_CHAR];	
	for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
		ssi_sprint (string, "class%02d", n_class);
		samples.addClassName (string);
	}

	Evaluation eval;
	NaiveBayes *model = ssi_pcast (NaiveBayes, Factory::Create (NaiveBayes::GetCreateName ()));
	Trainer trainer (model);
	trainer.train (samples);

	Evaluation2Latex e2latex;
	e2latex.open ("eval.tex");
	
	ssi_print ("devel set:\n");
	eval.eval (trainer, samples);
	eval.print ();
	eval.print_result_vec ();

	e2latex.writeHead (eval, "caption", "label");
	e2latex.writeText ("results with different evaluation strategies", true);
	e2latex.writeEval ("devel", eval);
	
	ssi_print ("k-fold:\n");
	eval.evalKFold (trainer, samples, 3); 
	eval.print ();
	eval.print_result_vec ();

	e2latex.writeEval ("k-fold", eval);

	ssi_print ("split:\n");
	eval.evalSplit (trainer, samples, 0.5f); 
	eval.print ();
	eval.print_result_vec ();

	e2latex.writeEval ("split", eval);

	ssi_print ("loo:\n");
	eval.evalLOO (trainer, samples); 
	eval.print ();
	eval.print_result_vec ();

	e2latex.writeEval ("loo", eval);
	
	e2latex.writeTail ();
	e2latex.close ();
}

void ex_model () {

	Trainer::SetLogLevel (SSI_LOG_LEVEL_DEBUG);

	ssi_size_t n_classes = 4;
	ssi_size_t n_samples = 50;
	ssi_size_t n_streams = 1;
	ssi_real_t train_distr[][3] = { 0.3f, 0.3f, 0.2f, 0.3f, 0.6f, 0.2f, 0.6f, 0.3f, 0.2f, 0.6f, 0.6f, 0.2f };
	ssi_real_t test_distr[][3] = { 0.5f, 0.5f, 0.5f };
	SampleList strain;
	SampleList sdevel;
	SampleList stest;
	ModelTools::CreateTestSamples (strain, n_classes, n_samples, n_streams, train_distr, "user");	
	ModelTools::CreateTestSamples (sdevel, n_classes, n_samples, n_streams, train_distr, "user");	
	ModelTools::CreateTestSamples (stest, 1, n_samples * n_classes, n_streams, test_distr, "user");	
	ssi_char_t string[SSI_MAX_CHAR];	
	for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
		ssi_sprint (string, "class%02d", n_class);
		stest.addClassName (string);
	}

	// train knn
	{
		KNearestNeighbors *model = ssi_pcast (KNearestNeighbors, Factory::Create (KNearestNeighbors::GetCreateName ()));
		model->getOptions ()->k = 5;
		//model->getOptions ()->distsum = true;
		Trainer trainer (model);
		trainer.train (strain);
		trainer.save ("knn");
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "knn");			
		Evaluation eval;
		eval.eval (trainer, sdevel);
		eval.print ();

		trainer.cluster (stest);
		PlotSamples (stest, "knn");
	}

	// train naive bayes
	{
		NaiveBayes *model = ssi_pcast (NaiveBayes, Factory::Create (NaiveBayes::GetCreateName ()));
		model->getOptions ()->log = true;
		Trainer trainer (model);
		trainer.train (strain);
		trainer.save ("bayes");
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "bayes");			
		Evaluation eval;
		eval.eval (trainer, sdevel);
		eval.print ();

		trainer.cluster (stest);
		PlotSamples (stest, "bayes");
	}

	// train svm
	{
		SVM *model = ssi_pcast (SVM, Factory::Create (SVM::GetCreateName ()));
		Trainer trainer (model);
		trainer.train (strain);
		trainer.save ("svm");
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "svm");			
		Evaluation eval;
		eval.eval (trainer, sdevel);
		eval.print ();

		trainer.cluster (stest);
		PlotSamples (stest, "svm");
	}

	// training
	{
		LDA *model = ssi_factory_create (LDA, "lda", true);		
		Trainer trainer (model);
		trainer.train (strain);

		model->print ();
		trainer.save ("lda");
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "lda");
		Evaluation eval;
		eval.eval (trainer, sdevel);
		eval.print ();

		trainer.cluster (stest);
		PlotSamples (stest, "lda");
	}

	ssi_print ("\n\n\tpress a key to contiue\n");
	getchar ();

}

void ex_fusion () {

	ssi_tic ();

	ssi_size_t n_classes = 4;
	ssi_size_t n_samples = 50;
	ssi_size_t n_streams = 3;
	ssi_real_t train_distr[][3] = { 0.3f, 0.3f, 0.2f, 0.3f, 0.6f, 0.2f, 0.6f, 0.3f, 0.2f, 0.6f, 0.6f, 0.2f };
	ssi_real_t test_distr[][3] = { 0.5f, 0.5f, 0.5f };
	SampleList strain;
	SampleList sdevel;
	SampleList stest;
	ModelTools::CreateTestSamples (strain, n_classes, n_samples, n_streams, train_distr, "user");			
	ModelTools::CreateTestSamples (sdevel, n_classes, n_samples, n_streams, train_distr, "user");	
	ModelTools::CreateTestSamples (stest, 1, n_samples * n_classes, n_streams, test_distr, "user");	
	ssi_char_t string[SSI_MAX_CHAR];	
	for (ssi_size_t n_class = 1; n_class < n_classes; n_class++) {
		ssi_sprint (string, "class%02d", n_class);
		stest.addClassName (string);
	}

	ssi_char_t *name = "simpfuse";

	// strain
	{
		IModel **models = new IModel *[n_streams];
		ssi_char_t string[SSI_MAX_CHAR];
		for (ssi_size_t n_stream = 0; n_stream < n_streams; n_stream++) {
			ssi_sprint (string, "%s.%02d", name, n_stream);
			models[n_stream] = ssi_pcast (IModel, Factory::Create (SimpleKNN::GetCreateName (), string));
		}
		SimpleFusion *fusion = ssi_pcast (SimpleFusion, Factory::Create (SimpleFusion::GetCreateName (), name));

		Trainer trainer (n_streams, models, fusion);
		trainer.train (strain);
		trainer.save ("simpfuse");

		delete[] models;
	}

	// evaluation
	{
		Trainer trainer;
		Trainer::Load (trainer, "simpfuse");					
		Evaluation eval;
		eval.eval (trainer, sdevel);
		eval.print ();
	}

	ssi_toc_print ();
}


void CreateMissingData (SampleList &samples, double prob) {

	ssi_size_t n_streams = samples.getStreamSize ();
	ssi_sample_t *sample = 0;
	samples.reset ();
	while (sample = samples.next ()) {
		for (ssi_size_t nstrm = 0; nstrm < n_streams; nstrm++) {
			if (ssi_random () > prob) {							
				ssi_stream_reset (*sample->streams[nstrm]);
			}
		}
	}
	samples.setMissingData (true);
}

void PlotSamples (ISamples &samples, const ssi_char_t *name) {

	IThePainter *painter = Factory::GetPainter ();

	int plot_id = painter->AddCanvas (name);

	ssi_size_t n_classes = samples.getClassSize ();	
	PaintData<ssi_real_t> **plots = new PaintData<ssi_real_t> *[n_classes];
	
	const ssi_real_t min[2] = {-0.20f, -0.2f};
	const ssi_real_t max[2] = {1.2f, 1.2f};
	for (ssi_size_t n_class = 0; n_class < n_classes; n_class++) {

		plots[n_class] = new PaintData<ssi_real_t> (2, PAINT_TYPE_SCATTER);

		ssi_size_t n_samples = samples.getSize (n_class);
		ssi_size_t n_streams = samples.getStreamSize ();
		ssi_real_t *data = new ssi_real_t[n_samples * 2 * n_streams];

		ISSelectClass samples_sel (&samples);
		samples_sel.setSelection (n_class);
		samples_sel.reset ();
		ssi_sample_t *sample = 0;
		ssi_real_t *data_ptr = data;
		while (sample = samples_sel.next ()) {
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

	ssi_print ("\n\n\tpress enter to continue\n\n");
	getchar ();

	for (ssi_size_t n_class = 0; n_class < n_classes; n_class++) {
		delete plots[n_class];
	}
	delete[] plots;

	painter->Clear ();
}

