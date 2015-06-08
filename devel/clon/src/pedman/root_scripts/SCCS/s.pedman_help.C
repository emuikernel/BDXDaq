h58143
s 00001/00001/00054
d D 1.2 02/08/28 20:44:38 boiarino 3 1
c minor
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/s/pedman_help.C
e
s 00055/00000/00000
d D 1.1 00/07/31 15:01:43 gurjyan 1 0
c CodeManager Uniquification: pedman/s/pedman_help.C
c date and time created 00/07/31 15:01:43 by gurjyan
e
u
U
f e 0
t
T
I 1
{
//=========Macro generated from canvas: c1/CLAS PEDestal MANager
//=========  (Mon Jan 19 12:00:52 1998) by ROOT version 1.03/09
  gROOT->Reset();
c1->Clear();
   c1 = new TCanvas("c1","CLAS PEDestal MANager",150,0,700,730);
   c1->SetHighLightColor(1);
   c1->Range(0,0,1,1);
   c1->SetFillColor(37);
//   c1->SetFillStyle(3008);
   c1->SetBorderSize(2);

   
   TPaveText *pt = new TPaveText(0.08,0.88,0.86,0.98,"br");
   pt->SetFillColor(42);
   pt->SetFillStyle(3007);
   pt->SetTextColor(12);
   pt->SetTextFont(72);
   TText *text = pt->AddText("CLAS Pedman Help");
   pt->Draw();
   
   pt = new TPaveText(0.05,0.05,0.95,0.848,"br");
   pt->SetFillColor(5);
   pt->SetFillStyle(1);
   pt->SetTextAlign(12);
   pt->SetTextFont(211);
   pt->SetTextSize(0.028689);
   text = pt->AddText("xx - is the name of the detector component:");
D 3
   text = pt->AddText("ec1,ec2,sc,cc,lac. To see the individual histogram");
E 3
I 3
   text = pt->AddText("ec1,ec2,sc1,cc1,lac1. To see the individual histogram");
E 3
   text = pt->AddText("and fit, type the following in the ROOT command line:");
   text = pt->AddText("root[1] xx->Draw(\"value\",\"slot == # && channel ==#\");");
   text = pt->AddText("root[2] hist=new TH1F(\"hist\",\"name of hist\",#chan,#from,#to);");
   text = pt->AddText("root[3] xx->Draw(\"value>>hist\",\"slot == # && channel ==#\");");
   text = pt->AddText("root[4] hist->Draw();");
   text = pt->AddText("root[5] hist->fit(\"gaus\");");
   text = pt->AddText(" ");
   text = pt->AddText(" ");
   text = pt->AddText(" ");
   text = pt->AddText(" ");
   text->SetTextColor(2);
   text->SetTextFont(72);
   text = pt->AddText(".... Warning:");
   text = pt->AddText("Be sure previously you click on \"Root xx\" button");
   text->SetTextColor(2);
   text->SetTextFont(72);
   text = pt->AddText("     Use left button to rotate the 3d PEDMAN graphs");
   text->SetTextColor(4);
   text->SetTextFont(8);
   text = pt->AddText(".... Use right button to get an object sensitive pop-up");
   text->SetTextColor(2);
   text->SetTextFont(72);
   pt->Draw();
//   c1->Modified();
//   c1->cd();
}
E 1
