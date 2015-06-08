/*

input:
  v1 - income electron momentum (3d)
  v2 - virtual photon momentum (3d)
  v3 - adron (positive) (pion) momentum (3d)

  v2(4dim) = v4sub(PIncomingElectron, PScatteredelectron)

output:
  sinus of phi* (angle between secondary electron plane and pion plane)

*/

#define TWOPI  2.*3.14159265358979322702
#define PHIEPS 0.000001
#define MODULE(x) sqrt(x[0]*x[0]+x[1]*x[1]+x[2]*x[2])
#define ANGLE(v1,v2) acos(vdot_(v1,v2,&three)/(MODULE(v1)*MODULE(v2)))

/*
vector4_t Photon,PIncomingElectron,PScatteredelectron,Phadron;
float sinphistar;

Photon = v4sub(PIncomingElectron, PScatteredelectron);   
SinAzimuthalPhiStar(PIncomingElectron.space, Photon.space, Phadron.space);

vector4_t v4sub(vector4_t p1,vector4_t p2)
{
  vector4_t diff;

  diff.t = p1.t - p2.t;
  diff.space.x = p1.space.x - p2.space.x;
  diff.space.y = p1.space.y - p2.space.y;
  diff.space.z = p1.space.z - p2.space.z;

  return diff;
}
*/

float
SinAzimuthalPhiStar(float v1[3], float v2[3], float v3[3])
{
  float normal2QB[3], normal2QP[3];
  float phist, three=3;

  cross_(v2,v1,normal2QB);
  cross_(v2,v3,normal2QP); 
  if(MODULE(normal2QB) > PHIEPS && MODULE(normal2QP) > PHIEPS)
  {
    phist = ANGLE(normal2QB,normal2QP);
    if(vdot_(normal2QB,v3,&three) < 0.0) phist = TWOPI - phist;
  }

  if(phist > 0 && phist < TWOPI) return(sin(phist));
  return(0.);
}


