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
   text = pt->AddText("ec1,ec2,sc1,cc1,lac1. To see the individual histogram");
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
