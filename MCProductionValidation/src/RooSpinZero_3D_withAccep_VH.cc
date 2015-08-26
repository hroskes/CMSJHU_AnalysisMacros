/***************************************************************************** 
  * Project: RooFit                                                           * 
  *                                                                           * 
  * This code was autogenerated by RooClassFactory                            * 
  *****************************************************************************/ 

 // Your description goes here... 

#include "Riostream.h" 

#include "../include/RooSpinZero_3D_withAccep_VH.h" 

RooSpinZero_3D_withAccep_VH::RooSpinZero_3D_withAccep_VH(const char *name, const char *title,
  RooAbsReal& _h1,
  RooAbsReal& _h2,
  RooAbsReal& _Phi,
  RooAbsReal& _sqrts,
  RooAbsReal& _mX,
  RooAbsReal& _mV,
  RooAbsReal& _R1Val,
  RooAbsReal& _R2Val,
  int _parameterization,
  RooAbsReal& _a1Val,
  RooAbsReal& _phi1Val,
  RooAbsReal& _a2Val,
  RooAbsReal& _phi2Val,
  RooAbsReal& _a3Val,
  RooAbsReal& _phi3Val,
  RooAbsReal& _g1Val,
  RooAbsReal& _g2Val,
  RooAbsReal& _g3Val,
  RooAbsReal& _g4Val,
  RooAbsReal& _g1ValIm,
  RooAbsReal& _g2ValIm,
  RooAbsReal& _g3ValIm,
  RooAbsReal& _g4ValIm,
  RooAbsReal& _fa2,
  RooAbsReal& _fa3,
  RooAbsReal& _phia2,
  RooAbsReal& _phia3,
  RooAbsReal& _b2,
  RooAbsReal& _cgaus,
  RooAbsReal& _sgaus,
  bool _withAcc
  ) : RooSpinZero(
  name, title,
  _h1, _h2, _Phi,
  _mX, _mV, _R1Val, _R2Val,
  _parameterization,
  _a1Val, _phi1Val, _a2Val, _phi2Val, _a3Val, _phi3Val,
  _g1Val, _g2Val, _g3Val, _g4Val, _g1ValIm, _g2ValIm, _g3ValIm, _g4ValIm,
  _fa2, _fa3, _phia2, _phia3
  ),
  sqrts("sqrts", "sqrts", this, _sqrts),
  b2("b2", "b2", this, _b2),
  cgaus("cgaus", "cgaus", this, _cgaus),
  sgaus("sgaus", "sgaus", this, _sgaus),
  withAcc(_withAcc)
{}


RooSpinZero_3D_withAccep_VH::RooSpinZero_3D_withAccep_VH(
  const RooSpinZero_3D_withAccep_VH& other, const char* name
  ) : RooSpinZero(other, name),
  sqrts("sqrts", this, other.sqrts),
  b2("b2", this, other.b2),
  cgaus("cgaus", this, other.cgaus),
  sgaus("sgaus", this, other.sgaus),
  withAcc(other.withAcc)
{}



Double_t RooSpinZero_3D_withAccep_VH::evaluate() const
{

  // these amplitudes are calculated based on comparing equations to the PRD paper
  // the beta and gamma are velocities of the Z in the C.O.M
  // Double_t betaValSquared =(pow(sqrts,2)-(pow(mX+mV,2)))*(pow(sqrts,2)-(pow(mX-mV,2)))/pow(pow(sqrts,2)-mX*mX+mV*mV,2);
  // Double_t gamma = 1./sqrt(1-betaValSquared);
  // http://prd.aps.org/pdf/PRD/v49/i1/p79_1
  // Double_t f00 = gamma*gamma/(2+gamma*gamma);


  // check whether event is in acceptance or not

  vector<TLorentzVector> lep_4vecs = Calculate4Momentum(sqrts, mV, mX, acos(h1), acos(h2), acos(0), Phi, 0);

  double pt_plus = lep_4vecs[1].Pt();
  double pt_minus = lep_4vecs[0].Pt();
  double eta_plus = lep_4vecs[1].Eta();
  double eta_minus = lep_4vecs[0].Eta();

  if (withAcc) {
    if (pt_minus<5.0 || pt_plus<5.0 || eta_minus>2.4 || eta_plus>2.4 || eta_minus<-2.4 || eta_plus<-2.4) return 1e-15;
  }


  //-------------------------------------------------

  // below calcualtions are based on the H->ZZ amplitudes 
  double s=-(mX*mX-sqrts*sqrts-mV*mV)/2.;
  double kappa=s/(1000*1000);

  double a1=0, a2=0, a3=0, a1Im=0, a2Im=0, a3Im=0;
  double g1(1.0), g1Im(0.), g2(0.), g2Im(0.), g3(0.), g3Im(0.), g4(0.), g4Im(0.);

  if (parameterization==kMagPhase_As){
    a1=a1Val;
    a1Im=phi1Val;
    a2=a2Val;
    a2Im=phi2Val;
    a3=a3Val;
    a3Im=phi3Val;
  }
  else {

    if (parameterization==kFracPhase_Gs){

      double nanval = sqrt(1 - fa2 - fa3);
      if (nanval != nanval) return 0.0;


      // convert fraction and phase to g1,g2...etc
      // ILC numbers at 250 GeV at mH= 125 GeV (narrow Z width approximation)
      Double_t sigma1_e = 0.981396; // was 0.94696 at 126 GeV
      Double_t sigma2_e = 33.4674;  // was 32.1981 at 126 GeV
      Double_t sigma4_e = 7.9229;   // was 7.45502 at 126 GeV

      // ILC nubmers at 350 GeV at mH = 125 GeV
      if (sqrts == 350.) {
        sigma1_e = 1.48872;
        sigma2_e = 125.387;
        sigma4_e = 75.3199;
      }

      // ILC nubmers at 500 GeV at mH = 125 GeV
      if (sqrts == 500.) {
        sigma1_e = 2.57246;
        sigma2_e = 516.556;
        sigma4_e = 414.378;
      }

      // ILC nubmers at 1000 GeV at mH = 125 GeV
      if (sqrts == 1000.) {
        sigma1_e = 8.95721;
        sigma2_e = 8208.91;
        sigma4_e = 7800.2;
      }

      double g1Mag = 1.;
      double g2Mag = 0.;
      double g4Mag = 0.;

      if (fa2 == 1.) {
        g1Mag = 0.;
        g2Mag = 1.;
        g4Mag = 0.;
      }
      else if (fa3 == 1.) {
        g1Mag = 0.;
        g2Mag = 0.;
        g4Mag = 1.;
      }
      else {
        g2Mag = sqrt(fa2/(1.-fa2-fa3))*sqrt(sigma1_e/sigma2_e);
        g4Mag = sqrt(fa3/(1.-fa2-fa3))*sqrt(sigma1_e/sigma4_e);
      }

      g1   = g1Mag;
      g1Im = 0.0;
      g2   = -g2Mag*cos(phia2);
      g2Im = -g2Mag*sin(phia2);
      g3   = 0.0;
      g3Im = 0.0;
      g4   = -g4Mag*cos(phia3);
      g4Im = -g4Mag*sin(phia3);


    }
    else if (parameterization==kRealImag_Gs){

      g1   =  g1Val;
      g1Im =  g1ValIm;
      g2   =  -g2Val;
      g2Im =  -g2ValIm;
      g3   = g3Val;
      g3Im =  g3ValIm;
      g4   = -g4Val;
      g4Im = -g4ValIm;
    }

    a1 = g1*mV*mV/(mX*mX) + g2*2.*s/(mX*mX) + g3*kappa*s/(mX*mX);
    a1Im = g1Im*mV*mV/(mX*mX) + g2Im*2.*s/(mX*mX) + g3Im*kappa*s/(mX*mX);
    a2 = -2.*g2 - g3*kappa;
    a2Im = -2.*g2Im - g3Im*kappa;
    a3 = -2.*g4;
    a3Im = -2.*g4Im;

  }


  Double_t x = pow((mX*mX-sqrts*sqrts-mV*mV)/(2.*sqrts*mV), 2)-1;

  Double_t A00Real = -(a1*sqrt(1+x) + a2*(mV*sqrts)/(mX*mX)*x);
  Double_t A00Imag = -(a1Im*sqrt(1+x) + a2Im*(mV*sqrts)/(mX*mX)*x);

  Double_t Ap0Real = a1 - a3Im*(mV*sqrts)/(mX*mX)*sqrt(x);
  Double_t Ap0Imag = a1Im + a3*(mV*sqrts)/(mX*mX)*sqrt(x);

  Double_t Am0Real = a1 + a3Im*(mV*sqrts)/(mX*mX)*sqrt(x);
  Double_t Am0Imag = a1Im - a3*(mV*sqrts)/(mX*mX)*sqrt(x);

  Double_t f00 = A00Real*A00Real + A00Imag*A00Imag;
  Double_t fp0 = Ap0Real*Ap0Real + Ap0Imag*Ap0Imag;
  Double_t fm0 = Am0Real*Am0Real + Am0Imag*Am0Imag;

  Double_t ftotal = f00 + fp0 + fm0;

  // normalize to the total
  f00 = f00 / ftotal;
  fp0 = fp0 / ftotal;
  fm0 = fm0 / ftotal;

  Double_t phi00=atan2(A00Imag, A00Real);
  Double_t phip0=atan2(Ap0Imag, Ap0Real)-phi00;
  Double_t phim0=atan2(Am0Imag, Am0Real)-phi00;

  Double_t value = 0;


  value += (f00*(-1 + Power(h1, 2))*(-1 + Power(h2, 2)))/4.;

  value += (fp0*(1 + Power(h1, 2) - 2*h1*R1Val)*(1 + Power(h2, 2) + 2*h2*R2Val))/16.;

  value += (fm0*(1 + Power(h1, 2) + 2*h1*R1Val)*(1 + Power(h2, 2) - 2*h2*R2Val))/16.;

  value += -(Sqrt(f00)*Sqrt(fp0)*Sqrt(1 - Power(h1, 2))*Sqrt(1 - Power(h2, 2))*(h1 - R1Val)*(h2 + R2Val)*Cos(Phi + phip0))/4.;

  value += -(Sqrt(f00)*Sqrt(fm0)*Sqrt(1 - Power(h1, 2))*Sqrt(1 - Power(h2, 2))*(h1 + R1Val)*(h2 - R2Val)*Cos(Phi - phim0))/4.;

  value += (Sqrt(fm0)*Sqrt(fp0)*(-1 + Power(h1, 2))*(-1 + Power(h2, 2))*Cos(2*Phi - phim0 + phip0))/8.;


  // define acceptance terms

  Double_t acc = 1;

  if (withAcc) {
    // acc = ( 1 + Power(h1,2) * b2 ) *(1 +  cgaus*( Exp(-Phi*Phi/sgaus) + Exp(-Power(Abs(Phi)-Pi(),2)/sgaus) ) ) ;
    acc = (1 + Power(h1, 2) * b2 +  cgaus*(Exp(-Phi*Phi/sgaus) + Exp(-Power(Abs(Phi)-Pi(), 2)/sgaus)));
  }

  return value*acc;
}

Int_t RooSpinZero_3D_withAccep_VH::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars, const char* /*rangeName*/) const
{

  if (!withAcc) {
    if (matchArgs(allVars, analVars, RooArgSet(*h1.absArg(), *h2.absArg(), *Phi.absArg()))) return 4;
    if (matchArgs(allVars, analVars, h1, h2)) return 1;
    if (matchArgs(allVars, analVars, h1, Phi)) return 2;
    if (matchArgs(allVars, analVars, h2, Phi)) return 3;
  }

  return 0;
}

Double_t RooSpinZero_3D_withAccep_VH::analyticalIntegral(Int_t code, const char* rangeName) const
{

  // check whether event is in acceptance or not

  vector<TLorentzVector> lep_4vecs = Calculate4Momentum(sqrts, mV, mX, acos(h1), acos(h2), acos(0), Phi, 0);

  double pt_plus = lep_4vecs[1].Pt();
  double pt_minus = lep_4vecs[0].Pt();
  double eta_plus = lep_4vecs[1].Eta();
  double eta_minus = lep_4vecs[0].Eta();

  if (withAcc) {
    if (pt_minus<5.0 || pt_plus<5.0 || eta_minus>2.4 || eta_plus>2.4 || eta_minus<-2.4 || eta_plus<-2.4) return 1e-10;
  }

  // the beta and gamma are velocities of the Z in the C.O.M
  Double_t betaValSquared =(pow(sqrts, 2)-(pow(mX+mV, 2)))*(pow(sqrts, 2)-(pow(mX-mV, 2)))/pow(pow(sqrts, 2)-mX*mX+mV*mV, 2);
  Double_t gamma = 1./sqrt(1-betaValSquared);

  // these amplitudes are calculated based on comparing equations to the PRD paper
  // http://prd.aps.org/pdf/PRD/v49/i1/p79_1
  // Double_t f00 = gamma*gamma/(2+gamma*gamma);

  // below calcualtions are based on the H->ZZ amplitudes 
  double s=-(mX*mX-sqrts*sqrts-mV*mV)/2.;
  double kappa=s/(1000*1000);

  double a1=0, a2=0, a3=0, a1Im=0, a2Im=0, a3Im=0;
  double g1(1.0), g1Im(0.), g2(0.), g2Im(0.), g3(0.), g3Im(0.), g4(0.), g4Im(0.);

  if (parameterization==kMagPhase_As){
    a1=a1Val;
    a1Im=phi1Val;
    a2=a2Val;
    a2Im=phi2Val;
    a3=a3Val;
    a3Im=phi3Val;
  }
  else {

    if (parameterization==kFracPhase_Gs){
      // 
      // place holder, not correct
      // 
      double nanval = sqrt(1 - fa2 - fa3);
      if (nanval != nanval) return 0.0;

      // convert fraction and phase to g1,g2...etc
      // ILC numbers at 250 GeV at mH= 125 GeV (narrow Z width approximation)
      Double_t sigma1_e = 0.981396; // was 0.94696 at 126 GeV
      Double_t sigma2_e = 33.4674;  // was 32.1981 at 126 GeV
      Double_t sigma4_e = 7.9229;   // was 7.45502 at 126 GeV

      // ILC nubmers at 350 GeV at mH = 125 GeV
      if (sqrts == 350.) {
        sigma1_e = 1.48872;
        sigma2_e = 125.387;
        sigma4_e = 75.3199;
      }

      // ILC nubmers at 500 GeV at mH = 125 GeV
      if (sqrts == 500.) {
        sigma1_e = 2.57246;
        sigma2_e = 516.556;
        sigma4_e = 414.378;
      }

      // ILC nubmers at 1000 GeV at mH = 125 GeV
      if (sqrts == 1000.) {
        sigma1_e = 8.95721;
        sigma2_e = 8208.91;
        sigma4_e = 7800.2;
      }

      double g1Mag = 1.;
      double g2Mag = 0.;
      double g4Mag = 0.;

      if (fa2 == 1.) {
        g1Mag = 0.;
        g2Mag = 1.;
        g4Mag = 0.;
      }
      else if (fa3 == 1.) {
        g1Mag = 0.;
        g2Mag = 0.;
        g4Mag = 1.;
      }
      else {
        g2Mag = sqrt(fa2/(1.-fa2-fa3))*sqrt(sigma1_e/sigma2_e);
        g4Mag = sqrt(fa3/(1.-fa2-fa3))*sqrt(sigma1_e/sigma4_e);
      }

      g1   = g1Mag;
      g1Im = 0.0;
      g2   = -g2Mag*cos(phia2);
      g2Im = -g2Mag*sin(phia2);
      g3   = 0.0;
      g3Im = 0.0;
      g4   = -g4Mag*cos(phia3);
      g4Im = -g4Mag*sin(phia3);


    }
    else if (parameterization==kRealImag_Gs){

      g1   =  g1Val;
      g1Im =  g1ValIm;
      g2   =  -g2Val;
      g2Im =  -g2ValIm;
      g3   = g3Val;
      g3Im = g3ValIm;
      g4   = -g4Val;
      g4Im = -g4ValIm;
    }

    a1 = g1*mV*mV/(mX*mX) + g2*2.*s/(mX*mX) + g3*kappa*s/(mX*mX);
    a1Im = g1Im*mV*mV/(mX*mX) + g2Im*2.*s/(mX*mX) + g3Im*kappa*s/(mX*mX);
    a2 = -2.*g2 - g3*kappa;
    a2Im = -2.*g2Im - g3Im*kappa;
    a3 =  -2.*g4;
    a3Im = -2.*g4Im;

  }

  Double_t x = pow((mX*mX-sqrts*sqrts-mV*mV)/(2.*sqrts*mV), 2)-1;

  Double_t A00Real = -(a1*sqrt(1+x) + a2*(mV*sqrts)/(mX*mX)*x);
  Double_t A00Imag = -(a1Im*sqrt(1+x) + a2Im*(mV*sqrts)/(mX*mX)*x);

  Double_t Ap0Real = a1 - a3Im*(mV*sqrts)/(mX*mX)*sqrt(x);
  Double_t Ap0Imag = a1Im + a3*(mV*sqrts)/(mX*mX)*sqrt(x);

  Double_t Am0Real = a1 + a3Im*(mV*sqrts)/(mX*mX)*sqrt(x);
  Double_t Am0Imag = a1Im - a3*(mV*sqrts)/(mX*mX)*sqrt(x);

  Double_t f00 = A00Real*A00Real + A00Imag*A00Imag;
  Double_t fp0 = Ap0Real*Ap0Real + Ap0Imag*Ap0Imag;
  Double_t fm0 = Am0Real*Am0Real + Am0Imag*Am0Imag;

  Double_t ftotal = f00 + fp0 + fm0;

  // normalize to the total
  f00 = f00 / ftotal;
  fp0 = fp0 / ftotal;
  fm0 = fm0 / ftotal;

  Double_t phi00=atan2(A00Imag, A00Real);
  Double_t phip0=atan2(Ap0Imag, Ap0Real)-phi00;
  Double_t phim0=atan2(Am0Imag, Am0Real)-phi00;

  switch (code)
  {
    // projections to h2
  case 2:
  {


          double value = 0.;

          value += (-8*f00*(-1 + Power(h2, 2))*Power(Pi(), 2))/3.;

          value += (4*fp0*Power(Pi(), 2)*(1 + Power(h2, 2) + 2*h2*R2Val))/3.;

          value += (4*fm0*Power(Pi(), 2)*(1 + Power(h2, 2) - 2*h2*R2Val))/3.;

          return value/(4*Pi());

  }
    // projections to h1
  case 3:
  {

          double value = 0;

          value += (-8*f00*(-1 + Power(h1, 2))*Power(Pi(), 2))/3.;

          value += (4*fp0*Power(Pi(), 2)*(1 + Power(h1, 2) - 2*h1*R1Val))/3.;

          value += (4*fm0*Power(Pi(), 2)*(1 + Power(h1, 2) + 2*h1*R1Val))/3.;

          return value/(4*Pi());

  }
    // projections to Phi
  case 1:
  {


          double value = 0.;

          value += (16*f00*Pi())/9.;

          value += (16*fp0*Pi())/9.;

          value += (16*fm0*Pi())/9.;

          value += (Sqrt(f00)*Sqrt(fp0)*Power(Pi(), 3)*R1Val*R2Val*Cos(Phi + phip0))/4.;

          value += (Sqrt(f00)*Sqrt(fm0)*Power(Pi(), 3)*R1Val*R2Val*Cos(Phi - phim0))/4.;

          value += (8*sqrt(fm0)*sqrt(fp0)*Pi()*Cos(2*Phi - phim0 + phip0))/9.;

          return value/(4*Pi());

  }
    // projected everything
  case 4:
  {
          double value = 0.;

          value += (32*f00*Power(Pi(), 2))/9.;

          value += (32*fp0*Power(Pi(), 2))/9.;

          value += (32*fm0*Power(Pi(), 2))/9.;

          return value/(4*Pi());
  }
  }
  assert(0);
  return 0;
}

vector<TLorentzVector> RooSpinZero_3D_withAccep_VH::Calculate4Momentum(float Mx, float M1, float M2, float theta, float theta1, float theta2, float _Phi1_, float _Phi_) const
{

  float phi1, phi2;
  phi1=TMath::Pi()-_Phi1_;
  phi2=_Phi1_+_Phi_;


  float gamma1, gamma2, beta1, beta2;

  gamma1=(Mx*Mx+M1*M1-M2*M2)/(2*Mx*M1);
  gamma2=(Mx*Mx-M1*M1+M2*M2)/(2*Mx*M2);
  beta1=sqrt(1-1/(gamma1*gamma1));
  beta2=sqrt(1-1/(gamma2*gamma2));

  //gluon 4 vectors
  TLorentzVector p1CM(0, 0, Mx/2, Mx/2);
  TLorentzVector p2CM(0, 0, -Mx/2, Mx/2);

  //vector boson 4 vectors
  TLorentzVector kZ1(gamma1*M1*sin(theta)*beta1, 0, gamma1*M1*cos(theta)*beta1, gamma1*M1*1);
  TLorentzVector kZ2(-gamma2*M2*sin(theta)*beta2, 0, -gamma2*M2*cos(theta)*beta2, gamma2*M2*1);

  //Rotation and Boost matrices. Note gamma1*beta1*M1=gamma2*beta2*M2.

  TLorentzRotation Z1ToZ, Z2ToZ;

  Z1ToZ.Boost(0, 0, beta1);
  Z2ToZ.Boost(0, 0, beta2);
  Z1ToZ.RotateY(theta);
  Z2ToZ.RotateY(TMath::Pi()+theta);

  //fermons 4 vectors in vector boson rest frame

  TLorentzVector p3Z1((M1/2)*sin(theta1)*cos(phi1), (M1/2)*sin(theta1)*sin(phi1), (M1/2)*cos(theta1), (M1/2)*1);

  TLorentzVector p4Z1(-(M1/2)*sin(theta1)*cos(phi1), -(M1/2)*sin(theta1)*sin(phi1), -(M1/2)*cos(theta1), (M1/2)*1);

  TLorentzVector p5Z2((M2/2)*sin(theta2)*cos(phi2), (M2/2)*sin(theta2)*sin(phi2), (M2/2)*cos(theta2), (M2/2)*1);

  TLorentzVector p6Z2(-(M2/2)*sin(theta2)*cos(phi2), -(M2/2)*sin(theta2)*sin(phi2), -(M2/2)*cos(theta2), (M2/2)*1);


  // fermions 4 vectors in CM frame

  TLorentzVector p3CM, p4CM, p5CM, p6CM;

  p3CM=Z1ToZ*p3Z1;
  p4CM=Z1ToZ*p4Z1;
  p5CM=Z2ToZ*p5Z2;
  p6CM=Z2ToZ*p6Z2;

  vector<TLorentzVector> p;

  p.push_back(p3CM);
  p.push_back(p4CM);
  p.push_back(p5CM);
  p.push_back(p6CM);

  return p;
}


