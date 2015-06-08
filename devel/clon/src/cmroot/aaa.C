// displays root file contents
TFile hfile("hsimple.root");
TFile hfile("/tmp/root_server.root");
hfile.ls();

root [0] TFile hfile("hsimple.root");
root [1] hfile.ls();
TFile**         hsimple.root    Demo ROOT file with histograms
 TFile*         hsimple.root    Demo ROOT file with histograms
  KEY: TH1F     hpx;1   This is the px distribution
  KEY: TH2F     hpxpy;1 py vs px
  KEY: TProfile hprof;1 Profile of pz versus px
  KEY: TNtuple  ntuple;1        Demo ntuple
root [3] hpx->Draw();
<TCanvas::MakeDefCanvas>: created default TCanvas with name c1
