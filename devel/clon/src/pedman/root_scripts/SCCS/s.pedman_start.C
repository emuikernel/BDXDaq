h21283
s 00001/00001/00090
d D 1.2 02/08/28 20:51:55 boiarino 3 1
c bug fix: "Browser" -> \"Browser\"
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/pedman_start.C
e
s 00091/00000/00000
d D 1.1 00/07/31 15:01:43 gurjyan 1 0
c CodeManager Uniquification: pedman/s/pedman_start.C
c date and time created 00/07/31 15:01:43 by gurjyan
e
u
U
f e 0
t
T
I 1
{
   gROOT->Reset();

   c1 = new TCanvas("c1","CLAS PEDestal MANager",150,0,700,730);
   c1->SetHighLightColor(1);
   c1->SetFillColor(42);
   c1->SetBorderSize(2);
   
   TPavesText *pst = new TPavesText(0.0944882,0.768443,0.829396,0.922131,5,"br");
   pst->SetFillColor(33);
   pst->Draw();
   TText *text = new TText(0.272966,0.819672,"PEDMAN");
   text->SetTextColor(12);
   text->SetTextFont(32);
   text->SetTextSize(0.090164);
   text->Draw();
   
   TPaveText *pt = new TPaveText(0.0944882,0.383197,0.884514,0.625,"br");
   pt->SetFillColor(41);
   pt->SetFillStyle(3007);
   pt->Draw();
   
   pt = new TPaveText(0.711286,0.0389345,0.986877,0.141393,"br");
   pt->SetFillColor(21);
   pt->Draw();
   text = new TText(0.128609,0.538934,"ROOT based CLAS Pedestal");
   text->SetTextColor(2);
   text->SetTextFont(32);
   text->SetTextSize(0.053279);
   text->Draw();
   text = new TText(0.488189,0.473361,"Manager");
   text->SetTextAlign(22);
   text->SetTextColor(2);
   text->SetTextFont(32);
   text->SetTextSize(0.059426);
   text->Draw();
   text = new TText(0.755906,0.106557,"CLAS Online");
   text->SetTextFont(22);
   text->SetTextSize(0.028689);
   text->Draw();
   text = new TText(0.748031,0.0696722,"Jefferson Lab");
   text->SetTextFont(22);
   text->SetTextSize(0.028689);
   text->Draw();
     c1->Update();
   c1->Modified();
   c1->cd();


   gROOT->Reset("a");
   bar = new TControlBar("vertical");
   bar->AddButton("Old/Curr  EC1",".x ec1_chan_500.C", "Channel dependence of the ratio between old measured pedestals and currently measured pedestals for EC1 crate. New pedestals are measured using 500ns internal ADC test gate.");
   bar->AddButton("Old/Anal EC1",".x ec1_slot_spar.C", "Slot dependence of the ratio between old and analyzed sparsification thresholds for EC1 crate.");
   bar->AddButton("Old/Curr EC2",".x ec2_chan_500.C", "Channel dependence of the ratio between old measured pedestals and currently measured pedestals for EC2 crate. New pedestals are measured using 500ns internal ADC test gate. ");
   bar->AddButton("Old/Anal EC2",".x ec2_slot_spar.C", "Slot dependence of the ratio between old and analyzed sparsification thresholds for EC2 crate.");
   bar->AddButton("Old/Curr SC ",".x sc_chan_500.C", "Channel dependence of the ratio between old measured pedestals and currently measured pedestals for SC1 crate. New pedestals are measured using 500ns internal ADC test gate. ");
   bar->AddButton("Old/Anal SC ",".x sc_slot_spar.C", "Slot dependence of the ratio between old and analyzed sparsification thresholds for SC1 crate.");
   bar->AddButton("Old/Curr CC ",".x cc_chan_500.C", "Channel dependence of the ratio between old measured pedestals and currently measured pedestals for CC1 crate. New pedestals are measured using 500ns internal ADC test gate.");
   bar->AddButton("Old/Anal CC ",".x cc_slot_spar.C", "Slot dependence of the ratio between old and analyzed sparsification thresholds for CC1 crate.");
   bar->AddButton("Old/Curr LAC",".x lac_chan_500.C", "Channel dependence of the ratio between old measured pedestals and currently measured pedestals for LAC1 crate. New pedestals are measured using 500ns internal ADC test gate.");
   bar->AddButton("Old/Anal LAC",".x lac_slot_spar.C", "Slot dependence of the ratio between old and analyzed sparsification thresholds for LAC1 crate.");
   bar->AddButton("3D"," ", " ");
   bar->AddButton("P_shift EC1",".x ec1_peder.C", "Logarithm of the ratio between old measured pedestals and currently measured pedestals for EC1 crate. New pedestals are measured using 500ns gate.");
   bar->AddButton("Sigma EC1",".x ec1_siger.C", "Sigma of the pedestal distributions for EC1 crate");
   bar->AddButton("P_shift EC2",".x ec2_peder.C", "Logarithm of the ratio between old measured pedestals and currently measured pedestals for EC2 crate. New pedestals are measured using 500ns gate.");
   bar->AddButton("Sigma EC2",".x ec2_siger.C", "Sigma of the pedestal distributions for EC2 crate");
   bar->AddButton("P_shift SC ",".x sc_peder.C", "Logarithm of the ratio between old measured pedestals and currently measured pedestals for SC1 crate. New pedestals are measured using 500ns gate.");
   bar->AddButton("Sigma SC ",".x sc_siger.C", "Sigma of the pedestal distributions for SC1 crate");
   bar->AddButton("P_shift CC ",".x cc_peder.C", "Logarithm of the ratio between old measured pedestals and currently measured pedestals for CC1 crate. New pedestals are measured using 500ns gate.");
   bar->AddButton("Sigma CC1 ",".x cc_siger.C", "Sigma of the pedestal distributions for CC crate");
   bar->AddButton("P_shift LAC",".x lac_peder.C", "Logarithm of the ratio between old measured pedestals and currently measured pedestals for LAC1 crate. New pedestals are measured using 500ns gate.");
   bar->AddButton("Sigma LAC",".x lac_siger.C", "Sigma of the pedestal distributions for LAC1 crate");
   bar->AddButton(" "," ", " ");
   bar->AddButton("Root EC1",".x ec12root.C", " pedman");
   bar->AddButton("Root EC2",".x ec2root.C", " pedman");
   bar->AddButton("Root SC ",".x sc2root.C", " pedman");
   bar->AddButton("Root CC ",".x cc2root.C", " pedman");
   bar->AddButton("Root LAC",".x lac2root.C", " pedman");
D 3
   bar->AddButton("Browser",     "{b = new TBrowser("Browser");}", "Start the ROOT Browser");
E 3
I 3
   bar->AddButton("Browser",     "{b = new TBrowser(\"Browser\");}", "Start the ROOT Browser");
E 3
   bar->AddButton("Help",".x pedman_help.C", " Runs Help on program");
   bar->Show();
   gROOT->SaveContext();



}





E 1
